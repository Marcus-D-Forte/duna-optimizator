#include <duna_optimizer/cost_function_numerical.h>
#include <duna_optimizer/levenberg_marquadt.h>
#include <duna_optimizer/loss_function/geman_mcclure.h>
#include <duna_optimizer/model.h>
#include <gtest/gtest.h>

using Scalar = float;

// Function to be minimized
struct Model : public duna_optimizer::BaseModel<Scalar, Model> {
  Model(Scalar *x, Scalar *y) : data_x(x), data_y(y) {}
  // API simply has to override this method

  bool f(const Scalar *x, Scalar *residual, unsigned int index) override {
    residual[0] = data_y[index] - (x[0] * data_x[index]) / (x[1] + data_x[index]);
    return true;
  }

 private:
  const Scalar *const data_x;
  const Scalar *const data_y;
};

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  duna_optimizer::logger::setGlobalVerbosityLevel(duna_optimizer::L_DEBUG);
  return RUN_ALL_TESTS();
}

class SimpleModel : public testing::Test {
 public:
  SimpleModel() {
    cost = new duna_optimizer::CostFunctionNumerical<Scalar, 2, 1>(
        Model::Ptr(new Model(x_data, y_data)), 7);

    // auto loss = new duna::loss::GemmanMCClure<Scalar>(100.0);
    cost->setLossFunction(duna_optimizer::loss::GemmanMCClure<Scalar>::Ptr(
        new duna_optimizer::loss::GemmanMCClure<Scalar>(100.0)));
    optimizer.addCost(cost);
  }

  ~SimpleModel() { delete cost; }

 protected:
  duna_optimizer::LevenbergMarquadt<Scalar, 2> optimizer;
  duna_optimizer::CostFunctionNumerical<Scalar, 2, 1> *cost;
  Scalar x_data[7] = {0.038, 0.194, 0.425, 0.626, 1.253, 2.5, 3.70};
  Scalar y_data[7] = {0.05, 0.127, 0.094, 0.2122, 0.2729, 0.2665, 0.3317};
};

TEST_F(SimpleModel, InitialCondition0) {
  Scalar x0[] = {0.9, 0.2};

  optimizer.minimize(x0);

  EXPECT_NEAR(x0[0], 0.362, 0.01);
  EXPECT_NEAR(x0[1], 0.556, 0.01);
}

TEST_F(SimpleModel, InitialCondition1) {
  Scalar x0[] = {1.9, 1.5};
  optimizer.minimize(x0);

  EXPECT_NEAR(x0[0], 0.362, 0.01);
  EXPECT_NEAR(x0[1], 0.556, 0.01);
}