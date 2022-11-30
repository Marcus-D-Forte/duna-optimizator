#pragma once

#include <duna/models/point2plane.h>

namespace duna
{
    template <typename PointSource, typename PointTarget, typename Scalar>
    struct Point2Plane3DOF : public Point2Plane<PointSource, PointTarget, Scalar>
    {
        Point2Plane3DOF(const pcl::PointCloud<PointSource> &source_, const pcl::PointCloud<PointSource> &target_, const pcl::Correspondences &corrs_) : Point2Plane<PointSource, PointTarget, Scalar>(source_, target_, corrs_)
        {
        }

        void init(const Scalar *x) override
        {
            so3::convert3DOFParameterToMatrix(x, transform);
        }

        void setup(const Scalar *x) override
        {
            so3::convert3DOFParameterToMatrix(x, transform);
        }

        virtual bool f(const Scalar *x, Scalar *f_x, unsigned int index) override
        {
            const PointSource &src_pt = source.points[corrs[index].index_query];
            const PointTarget &tgt_pt = target.points[corrs[index].index_match];

            Eigen::Matrix<Scalar, 4, 1> src_(static_cast<Scalar>(src_pt.x), static_cast<Scalar>(src_pt.y), static_cast<Scalar>(src_pt.z), 1.0);
            Eigen::Matrix<Scalar, 4, 1> tgt_(static_cast<Scalar>(tgt_pt.x), static_cast<Scalar>(tgt_pt.y), static_cast<Scalar>(tgt_pt.z), 0.0);
            Eigen::Matrix<Scalar, 4, 1> tgt_normal_(static_cast<Scalar>(tgt_pt.normal_x), static_cast<Scalar>(tgt_pt.normal_y), static_cast<Scalar>(tgt_pt.normal_z), 0.0f);

            Eigen::Matrix<Scalar, 4, 1> &&warped_src_ = transform * src_;

            f_x[0] = (warped_src_ - tgt_).dot(tgt_normal_);
            return true;
        }

        virtual bool f_df(const Scalar *x, Scalar *f_x, Scalar *jacobian, unsigned int index) override
        {
            const PointSource &src_pt = source.points[corrs[index].index_query];
            const PointTarget &tgt_pt = target.points[corrs[index].index_match];

            Eigen::Matrix<Scalar, 4, 1> src_(static_cast<Scalar>(src_pt.x), static_cast<Scalar>(src_pt.y), static_cast<Scalar>(src_pt.z), 1.0);
            Eigen::Matrix<Scalar, 4, 1> tgt_(static_cast<Scalar>(tgt_pt.x), static_cast<Scalar>(tgt_pt.y), static_cast<Scalar>(tgt_pt.z), 0.0);
            Eigen::Matrix<Scalar, 4, 1> tgt_normal_(static_cast<Scalar>(tgt_pt.normal_x), static_cast<Scalar>(tgt_pt.normal_y), static_cast<Scalar>(tgt_pt.normal_z), 0.0f);

            Eigen::Matrix<Scalar, 4, 1> &&warped_src_ = transform * src_;
            tgt_normal_[3] = 0;
            f_x[0] = (warped_src_ - tgt_).dot(tgt_normal_);

            // Not sure why we multiply by two. Numerical Diff comparison suggested that. Assume small rotations!!!
            jacobian[0] = (tgt_normal_[2] * src_[1] - tgt_normal_[1] * src_[2]);
            jacobian[1] = (tgt_normal_[0] * src_[2] - tgt_normal_[2] * src_[0]);
            jacobian[2] = (tgt_normal_[1] * src_[0] - tgt_normal_[0] * src_[1]);
            return true;
        }

    private:
        using Point2Plane<PointSource, PointTarget, Scalar>::transform;
        using Point2Plane<PointSource, PointTarget, Scalar>::source;
        using Point2Plane<PointSource, PointTarget, Scalar>::target;
        using Point2Plane<PointSource, PointTarget, Scalar>::corrs;
    };
}