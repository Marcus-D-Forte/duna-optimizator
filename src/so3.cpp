#include "so3.hpp"

namespace so3
{
    template <typename Scalar>
    void param2Matrix(const Eigen::Matrix<float, 6, 1> &x, Eigen::Matrix<Scalar, 4, 4> &transform_matrix_)
    {

        transform_matrix_.setZero();
        transform_matrix_(0, 3) = x[0];
        transform_matrix_(1, 3) = x[1];
        transform_matrix_(2, 3) = x[2];
        transform_matrix_(3, 3) = 1;

        // Compute w from the unit quaternion
        Eigen::Quaternion<Scalar> q(0, x[3], x[4], x[5]);
        q.w() = static_cast<Scalar>(std::sqrt(1 - q.dot(q)));
        q.normalize();
        transform_matrix_.topLeftCorner(3, 3) = q.toRotationMatrix();
    }


    // Instantiation
    template void param2Matrix<double>(const Eigen::Matrix<float, 6, 1> &x, Eigen::Matrix<double, 4, 4> &transform_matrix_);
    template void param2Matrix<float>(const Eigen::Matrix<float, 6, 1> &x, Eigen::Matrix<float, 4, 4> &transform_matrix_);

}
