// A very basic QP example

TEST_CASE("Simple QP 1")
{
    // Set up problem data.
    Eigen::MatrixXd P(2, 2);
    Eigen::VectorXd q(2);
    Eigen::MatrixXd A(3, 2);
    Eigen::VectorXd l(3);
    Eigen::VectorXd u(3);

    P << 2, 0.5, 0.5, 1;
    q << 1, 1;
    l << 1, 0, 0;
    u << 1, 0.7, 0.7;
    A << 1, 1, 1, 0, 0, 1;

    // Formulate QP.
    OptimizationProblem qp;

    VectorX x = qp.addVariable("x", 2);

    qp.addConstraint(box(par(l), par(A) * x, par(u)));

    qp.addCostTerm(x.transpose() * par(P) * x);
    qp.addCostTerm(par(q).transpose() * x);

    // Create and initialize the solver instance.
    osqp::OSQPSolver solver(qp);
    solver.setAlpha(1.0);

    // Solve the problem and show solver output.
    solver.solve(true);

    fmt::print("{}", qp);
    fmt::print("{}", solver);

    Eigen::VectorXd x_val = eval(x);
    Eigen::Vector2d x_sol(0.3, 0.7);

    const double cost = x_val.transpose() * P * x_val + q.dot(x_val);

    fmt::print("Solution:\n {}\n", x_val);
    fmt::print("Cost:    \n {}\n", cost);

    REQUIRE(x_val.isApprox(x_sol, 1e-5));

    // ! The order in which the variables are added to the problem may vary, so this test fails randomly.
    // auto test_stream = std::ostringstream();
    // test_stream << solver;
    // REQUIRE(test_stream.str() == "Quadratic problem\nMinimize 0.5x'Px + q'x\nSubject to l <= Ax <= u\nWith:\n\nP:\n4 1\n1 2\n\nq:\n1\n1\n\nA:\n1 1\n1 0\n0 1\n\nl:\n1\n0\n0\n\nu:\n  1\n0.7\n0.7");
}

TEST_CASE("Simple QP 2")
{
    OptimizationProblem qp;

    VectorX x = qp.addVariable("x", 3);
    qp.addConstraint(equalTo(x.sum(), 1.));
    qp.addConstraint(box(-1., x, 1.));
    qp.addCostTerm((2. + x(1)) * x(1) + (1. + x(0)) * x(0) + (1. + x(0)) * x(1) + x(2) * (2. + x(2)) + x(2) * x(2));

    osqp::OSQPSolver solver(qp);

    fmt::print("{}\n", qp);
    fmt::print("{}\n", solver);

    solver.solve(true);

    Eigen::Vector3d x_eval = eval(x);

    REQUIRE((x_eval - Eigen::Vector3d(1., -1. / 3., 1. / 3.)).cwiseAbs().maxCoeff() <= 1e-3);
}

TEST_CASE("Simple QP 3")
{
    const size_t m = 15;
    const size_t n = 10;
    const size_t p = 5;
    Eigen::MatrixXd P(n, n);
    Eigen::MatrixXd G(m, n);
    Eigen::MatrixXd A(p, n);
    Eigen::VectorXd q(n);
    Eigen::VectorXd h(m);
    Eigen::VectorXd b(p);
    Eigen::VectorXd x_sol(n);

    x_sol << -1.68244521, 0.29769913, -2.38772183, -2.79986015, 1.18270433,
        -0.20911897, -4.50993526, 3.76683701, -0.45770675, -3.78589638;

    P << 10.73433257, -4.87682749, -3.1554569, -4.59975348, 2.10303633,
        -1.24351932, 3.03068619, -4.1495217, -2.96204342, 0.03625465,
        -4.87682749, 8.89694764, 3.81265901, -0.0371134, -1.04768409,
        4.17091308, 0.7686802, 0.33647291, 1.02832382, -3.82660609,
        -3.1554569, 3.81265901, 3.99593656, 1.16250737, 0.8306916,
        0.39898829, -0.77481, -1.986561, -0.56700588, -2.2059839,
        -4.59975348, -0.0371134, 1.16250737, 6.68472567, -0.24326262,
        -0.47384934, -1.55427149, 4.11423405, -1.79635811, 1.51965371,
        2.10303633, -1.04768409, 0.8306916, -0.24326262, 3.8996402,
        -4.61998109, 1.9657652, -1.64505037, -0.18314792, 0.349001,
        -1.24351932, 4.17091308, 0.39898829, -0.47384934, -4.61998109,
        11.83952896, -2.93009228, 0.82181014, -1.19149678, -1.56002577,
        3.03068619, 0.7686802, -0.77481, -1.55427149, 1.9657652,
        -2.93009228, 7.02029511, 1.05656854, 1.5246283, -3.3805852,
        -4.1495217, 0.33647291, -1.986561, 4.11423405, -1.64505037,
        0.82181014, 1.05656854, 10.69854949, 4.40928019, 0.4416819,
        -2.96204342, 1.02832382, -0.56700588, -1.79635811, -0.18314792,
        -1.19149678, 1.5246283, 4.40928019, 8.25519844, -1.81043647,
        0.03625465, -3.82660609, -2.2059839, 1.51965371, 0.349001,
        -1.56002577, -3.3805852, 0.4416819, -1.81043647, 6.69202585;

    G << -1.37311732e+00, 3.15159392e-01, 8.46160648e-01,
        -8.59515941e-01, 3.50545979e-01, -1.31228341e+00,
        -3.86955093e-02, -1.61577235e+00, 1.12141771e+00,
        4.08900538e-01, -2.46169559e-02, -7.75161619e-01,
        1.27375593e+00, 1.96710175e+00, -1.85798186e+00,
        1.23616403e+00, 1.62765075e+00, 3.38011697e-01,
        -1.19926803e+00, 8.63345318e-01, -1.80920302e-01,
        -6.03920628e-01, -1.23005814e+00, 5.50537496e-01,
        7.92806866e-01, -6.23530730e-01, 5.20576337e-01,
        -1.14434139e+00, 8.01861032e-01, 4.65672984e-02,
        -1.86569772e-01, -1.01745873e-01, 8.68886157e-01,
        7.50411640e-01, 5.29465324e-01, 1.37701210e-01,
        7.78211279e-02, 6.18380262e-01, 2.32494559e-01,
        6.82551407e-01, -3.10116774e-01, -2.43483776e+00,
        1.03882460e+00, 2.18697965e+00, 4.41364444e-01,
        -1.00155233e-01, -1.36444744e-01, -1.19054188e-01,
        1.74094083e-02, -1.12201873e+00, -5.17094458e-01,
        -9.97026828e-01, 2.48799161e-01, -2.96641152e-01,
        4.95211324e-01, -1.74703160e-01, 9.86335188e-01,
        2.13533901e-01, 2.19069973e+00, -1.89636092e+00,
        -6.46916688e-01, 9.01486892e-01, 2.52832571e+00,
        -2.48634778e-01, 4.36689932e-02, -2.26314243e-01,
        1.33145711e+00, -2.87307863e-01, 6.80069840e-01,
        -3.19801599e-01, -1.27255876e+00, 3.13547720e-01,
        5.03184813e-01, 1.29322588e+00, -1.10447026e-01,
        -6.17362064e-01, 5.62761097e-01, 2.40737092e-01,
        2.80665077e-01, -7.31127037e-02, 1.16033857e+00,
        3.69492716e-01, 1.90465871e+00, 1.11105670e+00,
        6.59049796e-01, -1.62743834e+00, 6.02319280e-01,
        4.20282204e-01, 8.10951673e-01, 1.04444209e+00,
        -4.00878192e-01, 8.24005618e-01, -5.62305431e-01,
        1.95487808e+00, -1.33195167e+00, -1.76068856e+00,
        -1.65072127e+00, -8.90555584e-01, -1.11911540e+00,
        1.95607890e+00, -3.26499498e-01, -1.34267579e+00,
        1.11438298e+00, -5.86523939e-01, -1.23685338e+00,
        8.75838928e-01, 6.23362177e-01, -4.34956683e-01,
        1.40754000e+00, 1.29101580e-01, 1.61694960e+00,
        5.02740882e-01, 1.55880554e+00, 1.09402696e-01,
        -1.21974440e+00, 2.44936865e+00, -5.45774168e-01,
        -1.98837863e-01, -7.00398505e-01, -2.03394449e-01,
        2.42669441e-01, 2.01830179e-01, 6.61020288e-01,
        1.79215821e+00, -1.20464572e-01, -1.23312074e+00,
        -1.18231813e+00, -6.65754518e-01, -1.67419581e+00,
        8.25029824e-01, -4.98213564e-01, -3.10984978e-01,
        -1.89148284e-03, -1.39662042e+00, -8.61316361e-01,
        6.74711526e-01, 6.18539131e-01, -4.43171931e-01,
        1.81053491e+00, -1.30572692e+00, -3.44987210e-01,
        -2.30839743e-01, -2.79308500e+00, 1.93752881e+00,
        3.66332015e-01, -1.04458938e+00, 2.05117344e+00,
        5.85662000e-01, 4.29526140e-01, -6.06998398e-01;

    A << 0.83600472, 1.54335911, 0.75880566, 0.88490881, -0.87728152,
        -0.86778722, -1.44087602, 1.23225307, -0.25417987, 1.39984394,
        -0.78191168, -0.43750898, 0.09542509, 0.92145007, 0.0607502,
        0.21112476, 0.01652757, 0.17718772, -1.11647002, 0.0809271,
        -0.18657899, -0.05682448, 0.49233656, -0.68067814, -0.08450803,
        -0.29736188, 0.417302, 0.78477065, -0.95542526, 0.58591043,
        2.06578332, -1.47115693, -0.8301719, -0.8805776, -0.27909772,
        1.62284909, 0.01335268, -0.6946936, 0.6218035, -0.59980453,
        1.12341216, 0.30526704, 1.3887794, -0.66134424, 3.03085711,
        0.82458463, 0.65458015, -0.05118845, -0.72559712, -0.86776868;

    q << -0.44712856, 1.2245077, 0.40349164, 0.59357852, -1.09491185,
        0.16938243, 0.74055645, -0.9537006, -0.26621851, 0.03261455;

    h << -1.5874499, 2.2805469, -0.88969419, 0.52996943, 4.38507624,
        3.96464941, 1.11915806, -1.09999883, -1.46998049, -7.2213236,
        4.36425796, 3.13171944, -3.50538915, 3.15644261, -2.53542258;

    b << -0.13597733, -0.79726979, 0.28267571, -0.82609743, 0.6210827;

    OptimizationProblem op;

    VectorX x = op.addVariable("x", n);

    op.addConstraint(lessThan(par(G) * x, par(h)));
    op.addConstraint(equalTo(par(A) * x, par(b)));
    op.addCostTerm(x.transpose() * par(0.5 * P) * x + par(q).dot(x));

    osqp::OSQPSolver solver(op);

    solver.setPolish(true);
    solver.setEpsAbs(1e-5);
    solver.setEpsRel(1e-5);
    
    solver.solve(true);

    Eigen::VectorXd x_eval = eval(x);

    REQUIRE((x_sol - x_eval).cwiseAbs().maxCoeff() < 1e-5);
}

TEST_CASE("Non-convex QP")
{
    OptimizationProblem qp;

    VectorX x = qp.addVariable("x", 3);

    qp.addConstraint(equalTo(x.sum(), 1.));
    qp.addConstraint(box(-1., x, 1.));

    Eigen::Matrix3d M;
    M.setZero();
    M.diagonal() << -3, -2, -1;

    qp.addCostTerm(x.transpose() * par(M) * x);

    fmt::print("{}\n", qp);

    REQUIRE_THROWS(osqp::OSQPSolver(qp));
}
