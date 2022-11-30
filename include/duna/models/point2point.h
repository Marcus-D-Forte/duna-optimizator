#pragma once

#include "duna/so3.h"
#include "duna/model.h"
#include <pcl/point_cloud.h>
#include <pcl/correspondence.h>

namespace duna
{
    template <typename PointSource, typename PointTarget, typename Scalar>
    struct Point2Point : public BaseModelJacobian<Scalar>
    {

        Point2Point(const pcl::PointCloud<PointSource> &source_,
                    const pcl::PointCloud<PointSource> &target_,
                    const pcl::Correspondences &corrs_) : source(source_),
                                                          target(target_),
                                                          corrs(corrs_)
        {
        }

        void setup(const Scalar *x) override
        {
            so3::convert6DOFParameterToMatrix(x, transform);
        }

        bool f(const Scalar *x, Scalar *f_x, unsigned int index) override
        {
            const PointSource &src_pt = source.points[corrs[index].index_query];
            const PointTarget &tgt_pt = target.points[corrs[index].index_match];

            Eigen::Matrix<Scalar, 4, 1> src_(src_pt.x, src_pt.y, src_pt.z, 1);
            Eigen::Matrix<Scalar, 4, 1> tgt_(tgt_pt.x, tgt_pt.y, tgt_pt.z, 0);

            Eigen::Matrix<Scalar, 4, 1> warped_src_ = transform * src_;
            warped_src_[3] = 0;
            f_x[0] = (warped_src_ - tgt_).norm();
            return true;
        }

        bool f_df(const Scalar *x, Scalar *f_x, Scalar *jacobian, unsigned int index) override
        {
            const PointSource &src_pt = source.points[corrs[index].index_query];
            const PointTarget &tgt_pt = target.points[corrs[index].index_match];

            Eigen::Matrix<Scalar, 4, 1> src_(src_pt.x, src_pt.y, src_pt.z, 1);
            Eigen::Matrix<Scalar, 4, 1> tgt_(tgt_pt.x, tgt_pt.y, tgt_pt.z, 0);

            Eigen::Matrix<Scalar, 4, 1> warped_src_ = transform * src_;
            warped_src_[3] = 0;
            f_x[0] = (warped_src_ - tgt_).norm();

            Eigen::Matrix<Scalar, 4, 1> error = warped_src_ - tgt_;

            Scalar dx = error[0];
            Scalar dy = error[1];
            Scalar dz = error[2];

            Scalar sqrErr = dx * dx + dy * dy + dz * dz;

            Scalar rho = (1 / sqrt(sqrErr));

            jacobian[0] = rho * dx;
            jacobian[1] = rho * dy;
            jacobian[2] = rho * dz;
            
            jacobian[3] = rho * (src_[1] * dz - src_[2] * dy);
            jacobian[4] = rho * (src_[2] * dx - src_[0] * dz);
            jacobian[5] = rho * (src_[0] * dy - src_[1] * dx);
            return true;
        }

    protected:
        const pcl::PointCloud<PointSource> &source;
        const pcl::PointCloud<PointTarget> &target;
        const pcl::Correspondences &corrs;
        Eigen::Matrix<Scalar, 4, 4> transform;
    };
}