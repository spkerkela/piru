#include "point.h"

Point cartesian_to_isometric(const Point cartesian_point) {
  Point isometric_point;
  isometric_point.x = (cartesian_point.x - cartesian_point.y) * TILE_WIDTH_HALF;
  isometric_point.y =
      (cartesian_point.x + cartesian_point.y) * TILE_HEIGHT_HALF;
  return isometric_point;
}

Point isometric_to_cartesian(const Point isometric_point) {
  Point cartesian_point;
  double xx;
  double yy;
  xx = (((double)isometric_point.x / TILE_WIDTH_HALF) +
        ((double)isometric_point.y / TILE_HEIGHT_HALF)) /
       2.0;
  yy = (((double)isometric_point.y / TILE_HEIGHT_HALF) -
        ((double)isometric_point.x / TILE_WIDTH_HALF)) /
       2.0;
  cartesian_point.x = (int)xx;
  cartesian_point.y = (int)yy;
  return cartesian_point;
}

Point get_tile_coordinates(const Point cartesian_point) {
  Point tile_coordinates;
  tile_coordinates.x = cartesian_point.x / TILE_WIDTH;
  tile_coordinates.y = cartesian_point.y / TILE_HEIGHT;
  return tile_coordinates;
}

double get_distance(const Point a, const Point b) {
  return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

bool point_equal(const Point a, const Point b) {
  return a.x == b.x && a.y == b.y;
}

Point get_direction_from_path(enum PATH_CODE code) {
  Point point;
  point.x = 0;
  point.y = 0;
  switch (code) {
  case UP_LEFT:
    point.x--;
    point.y--;
    break;
  case UP:
    point.y--;
    break;
  case UP_RIGHT:
    point.x++;
    point.y--;
    break;
  case LEFT:
    point.x--;
    break;
  case PATH_NONE:
    break;
  case RIGHT:
    point.x++;
    break;
  case DOWN_LEFT:
    point.x--;
    point.y++;
    break;
  case DOWN:
    point.y++;
    break;
  case DOWN_RIGHT:
    point.x++;
    point.y++;
    break;
  default:
    break;
  }

  return point;
}