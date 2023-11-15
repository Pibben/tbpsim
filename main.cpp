
#include <array>
#include <cmath>
#include <cfw.h>

struct Vec2 {
  double x;
  double y;

  Vec2() = default;

  Vec2(double a, double b) : x(a), y(b) {}

  Vec2(const Vec2&) = default;

  friend Vec2 operator-(const Vec2& other) {
    return {-other.x, -other.y};
  }

  friend Vec2 operator-(const Vec2& v1, const Vec2& v2) {
    return {v1.x - v2.x, v1.y - v2.y};
  }

  friend Vec2 operator+(const Vec2& v1, const Vec2& v2) {
    return {v1.x + v2.x, v1.y + v2.y};
  }

  Vec2& operator+=(const Vec2& v) {
    x += v.x;
    y += v.y;

    return *this;
  }

  friend Vec2 operator*(double s, const Vec2& v) {
    return {s * v.x, s * v.y};
  }

  friend Vec2 operator*(const Vec2& v, double s) {
    return {s * v.x, s * v.y};
  }

  void print() const {
    std::cout << x << ' ' << y << '\n';
  }
};

struct State {
  Vec2 x{};
  Vec2 v{};
};

struct Body {
  Body(Vec2 r, Vec2 rp) : s{r, rp} {}
  State s;
};

static double dist(Vec2 v1, Vec2 v2) {
  auto dx = v1.x - v2.x;
  auto dy = v1.y - v2.y;
  return std::sqrt(dx * dx + dy * dy);
}

static double dist(const Body& b1, const Body& b2) {
  return dist(b1.s.x, b2.s.x);
}

static Vec2 calcGravity(const Body& target, const Body& source) {
  return 1.0 / dist(target, source) * (source.s.x - target.s.x);
}

static Vec2 calcGravity(const Body& target, const Body& source1, const Body& source2) {
  auto g1 = calcGravity(target, source1);
  auto g2 = calcGravity(target, source2);

  return g1 + g2;
}

struct EulerSolver {
  void step(std::array<Body, 3>& bodies, double h) {
    auto g1 = calcGravity(bodies[0], bodies[1], bodies[2]);
    auto g2 = calcGravity(bodies[1], bodies[0], bodies[2]);
    auto g3 = calcGravity(bodies[2], bodies[1], bodies[0]);

    bodies[0].s.x += h * bodies[0].s.v;
    bodies[0].s.v += h * g1;

    bodies[1].s.x += h * bodies[1].s.v;
    bodies[1].s.v += h * g2;

    bodies[2].s.x += h * bodies[2].s.v;
    bodies[2].s.v += h * g3;
  }
};

struct RK4Solver {

};

struct LeapFrogSolver {

};

std::array<Body, 3> createIsosceles(double xp1, double yp1) {
  // https://arxiv.org/pdf/1303.0181.pdf
  double x1 = -1.0;
  double x2 = 1.0;
  double x3 = 0.0;

  double y1 = 0.0;
  double y2 = 0.0;
  double y3 = 0.0;

  double xp2 = xp1;
  double xp3 = -2.0 * xp1;
  double yp2 = yp1;
  double yp3 = -2.0 * yp1;

  return {Body{{x1, y1}, {xp1, yp1}}, Body{{x2, y2}, {xp2, yp2}}, Body{{x3, y3}, {xp3, yp3}}};
}

std::array<Body, 3> createIsosceles2(double x1, double x2, double v1, double v2) {
  // From https://arxiv.org/pdf/1705.00527v4.pdf
  Vec2 r1{x1, x2};
  Vec2 r2 = -r1;
  Vec2 r3{0.0, 0.0};

  Vec2 rp1{v1, v2};
  Vec2 rp2 = rp1;
  Vec2 rp3{-2.0 * v1, -2.0 * v2};

  return {Body{r1, rp1}, Body{r2, rp2}, Body{r3, rp3}};
}

using Canvas = std::vector<unsigned char>;

static void drawCircle(Canvas& canvas, Vec2 pos, double r) {
  int x0 = pos.x - r;
  int x1 = pos.x + r;
  int y0 = pos.y - r;
  int y1 = pos.y + r;

  for(int ix = x0; ix < x1; ++ix) {
    for(int iy = y0; iy < y1; ++iy) {
      double dx = ix - pos.x;
      double dy = iy - pos.y;
      double dist = std::sqrt(dx*dx + dy*dy);
      if(dist < r) {
        canvas[(iy * 1000 + ix) * 3] = 0xff;
        canvas[(iy * 1000 + ix) * 3 + 1] = 0xff;
        canvas[(iy * 1000 + ix) * 3 + 2] = 0xff;
      }
    }
  }
}

int main() {
  Canvas canvas(1000 * 800 * 3);

  cfw::Window disp1(1000, 800, "Disp1");

  bool going = true;
  disp1.setKeyCallback([&going](const cfw::Keys& key, bool pressed) {
    if (key == cfw::Keys::ESC || key == cfw::Keys::Q) {
      going = false;
    }
  });

  double h = 0.001;
  std::array<Body, 3> figure_eight = createIsosceles(0.30689, 0.12551);
  std::array<Body, 3> butterfly2 = createIsosceles2(1.0005576155, -0.0029240248, 0.3064392516, 0.1263673939);

  EulerSolver s;

  Vec2 offset{500, 400};
  double scale = 100;

  while(going) {
    std::fill(canvas.begin(), canvas.end(), 0);
    (figure_eight[0].s.x * scale + offset).print();
    drawCircle(canvas, figure_eight[0].s.x * scale + offset, 5);
    drawCircle(canvas, figure_eight[1].s.x * scale + offset, 5);
    drawCircle(canvas, figure_eight[2].s.x * scale + offset, 5);
    disp1.render(canvas.data(), 1000, 800);
    disp1.paint();
    s.step(figure_eight, 0.1);
    cfw::sleep(100);
  }
}
