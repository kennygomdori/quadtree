/**
 *
 * shifty quadtree (pa3)
 * sqtree.cpp
 * This file will be used for grading.
 *
 */

#include "sqtree.h"
#include <algorithm>    // std::max

// First Node constructor, given.
SQtree::Node::Node(pair<int,int> ul, int w, int h, RGBAPixel a, double v)
  :upLeft(ul),width(w),height(h),avg(a),var(v),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
{}

// Second Node constructor, given
SQtree::Node::Node(stats & s, pair<int,int> ul, int w, int h)
  :upLeft(ul),width(w),height(h),NW(NULL),NE(NULL),SE(NULL),SW(NULL) {
  avg = s.getAvg(ul,w,h);
  var = s.getVar(ul,w,h);
}

// SQtree destructor, given.
SQtree::~SQtree() {
  clear();
}

// SQtree copy constructor, given.
SQtree::SQtree(const SQtree & other) {
  copy(other);
}

// SQtree assignment operator, given.
SQtree & SQtree::operator=(const SQtree & rhs) {
  if (this != &rhs) {
    clear();
    copy(rhs);
  }
  return *this;
}

/**
 * SQtree constructor given tolerance for variance.
 */
SQtree::SQtree(PNG & imIn, double tol) {
  stats s = stats(imIn);
  pair<int,int> ul(0,0);
  // auto t = SQtree::subdivide(s, ul, imIn.width(), imIn.height());
  // cout << "first divide: " << t.first << ", " << t.second << endl;
  root = buildTree(s, ul, imIn.width(), imIn.height(), tol);
}

inline pair<int, int> SQtree::subdivide(stats & s, pair<int,int> & ul, int w, int h) {
  double min_var = s.getVar(ul, w, h);
  pair<int,int> min_pair;
  int x = ul.first;
  int y = ul.second;

  // minimizes the maximum variability among the (two or four) children.
  
  
  
  for (int dx = 0; dx < w; dx++) {
    for (int dy = 0; dy < h; dy++) {
      double var = std::max(
                 {s.getVar(pair<int,int>(x,      y     ),     dx,     dy),
                  s.getVar(pair<int,int>(x + dx, y     ), w - dx,     dy),
                  s.getVar(pair<int,int>(x,      y + dy),     dx, h - dy),
                  s.getVar(pair<int,int>(x + dx, y + dy), w - dx, h - dy)});
      if (var <= min_var) {
        min_var = var;
        min_pair = pair<int,int>(dx, dy);
      }
    }
  }
  return min_pair;
}


/**
 * Helper for the SQtree constructor.
 */
SQtree::Node * SQtree::buildTree(stats & s, pair<int,int> & ul, int w, int h, double tol) {

  if (w == 0 || h == 0) return NULL;
  Node * node = new Node(s, ul, w, h);

  int x0 = ul.first;
  int y0 = ul.second;
  if (node->var <= tol || (w == 1 && h == 1))
    return node;
  pair<int,int> cut_point = subdivide(s, ul, w, h);
  int dx = cut_point.first;
  int dy = cut_point.second;
  
  pair<int,int> ur(x0 + dx, y0     );
  pair<int,int> ll(x0     , y0 + dy);
  pair<int,int> lr(x0 + dx, y0 + dy);

  node->NW = SQtree::buildTree(s, ul,     dx,     dy, tol); // ptr to NW subtree
  node->NE = SQtree::buildTree(s, ur, w - dx,     dy, tol); // ptr to NE subtree
  node->SW = SQtree::buildTree(s, ll,     dx, h - dy, tol); // ptr to SW subtree
  node->SE = SQtree::buildTree(s, lr, w - dx, h - dy, tol); // ptr to SE subtree

  return node;

}
  
/**
 * Render SQtree and return the resulting image.
 */
PNG SQtree::render() {
  PNG img(root->width, root->height);
  paint(root, &img);
  return img;
}

/**
 * Delete allocated memory.
 */
void SQtree::clear() {
  clear_node(root);
}

void SQtree::copy(const SQtree & other) {
  root = copy_node(other.root);
}

int SQtree::size() {
  return tree_size(root);
}

int SQtree::tree_size(SQtree::Node* node) {
  if (node == NULL) return 0;
  return 1
    + tree_size(node->NW)
    + tree_size(node->NE)
    + tree_size(node->SE)
    + tree_size(node->SW);
}

SQtree::Node * SQtree::copy_node(SQtree::Node * node) {
  if (node == NULL) return NULL;
  Node *c = new Node(*node);
  c->NW = copy_node(node->NW); // ptr to NW subtree
  c->NE = copy_node(node->NE); // ptr to NE subtree
  c->SE = copy_node(node->SE); // ptr to SE subtree
  c->SW = copy_node(node->SW); // ptr to SW subtree
  return c;
}

void SQtree::paint(SQtree::Node* node, PNG* img) 
{ 
  if (node == NULL) 
      return;
  
  // if this node is a leaf, paint the node.
  if (node->NW == NULL && node->NE == NULL && node->SE == NULL && node->SW == NULL) {
    paintWithAverage(node, img);
    return;
  }

  // go down all branches
  paint(node->NW, img);
  paint(node->NE, img);
  paint(node->SE, img);
  paint(node->SW, img);
}

inline void SQtree::paintWithAverage(SQtree::Node*& node, PNG*& img) {
  int x1 = node->upLeft.first + node->width;
  int y1 = node->upLeft.second + node->height;
  
  for (int x = node->upLeft.first; x < x1; x++)
    for (int y = node->upLeft.second; y < y1; y++)
      *img->getPixel(x, y) = node->avg;
}

void SQtree::clear_node(SQtree::Node* node) 
{ 
  if (node == NULL) 
      return;

  // go down all branches
  clear_node(node->NW);
  clear_node(node->NE);
  clear_node(node->SE);
  clear_node(node->SW);

  delete node;
}
