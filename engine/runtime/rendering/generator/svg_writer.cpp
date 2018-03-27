#include <algorithm>
#include <map>
#include <memory>
#include <sstream>

#include "svg_writer.hpp"

using namespace generator;

namespace
{

std::string toColor(const gml::dvec3& c)
{
	std::stringstream ss;
	auto fn = [](double c) {
		c = gml::clamp(c, 0.0, 1.0);
		return static_cast<int>(255 * c);
	};

	ss << "rgb(" << fn(c[0]) << "," << fn(c[1]) << "," << fn(c[2]) << ")";

	return ss.str();
}
}

svg_writer_t::BaseElem::BaseElem(double z, const gml::dvec3& color)
	: z_{z}
	, color_{color}
{
}

svg_writer_t::BaseElem::~BaseElem()
{
}

svg_writer_t::VertexElem::VertexElem(const gml::dvec3& p, const gml::dvec3& color)
	: BaseElem{p[2], color}
	, p_{p}
{
}

void svg_writer_t::VertexElem::stream(std::ostream& os) const
{
	os << "<circle "
	   << "cx=\"" << p_[0] << "\" cy=\"" << p_[1] << "\" "
	   << "r=\"3\" style=\"fill: " << toColor(color_) << "\" />\n";
}

svg_writer_t::LineElem::LineElem(const gml::dvec3& p1, const gml::dvec3& p2, const gml::dvec3& color)
	: BaseElem{(p1[2] + p2[2]) / 2.0, color}
	, p1_(p1)
	, p2_(p2)
{
}

void svg_writer_t::LineElem::stream(std::ostream& os) const
{
	os << "<line "
	   << "x1=\"" << p1_[0] << "\" y1=\"" << p1_[1] << "\" "
	   << "x2=\"" << p2_[0] << "\" y2=\"" << p2_[1] << "\" "
	   << "style=\"stroke: " << toColor(color_) << ";\" />\n";
}

svg_writer_t::TriangleElem::TriangleElem(const gml::dvec3& p1, const gml::dvec3& p2, const gml::dvec3& p3,
										 const gml::dvec3& color)
	: BaseElem{(p1[2] + p2[2] + p3[2]) / 3.0, color}
	, p_{{p1, p2, p3}}
{
}

void svg_writer_t::TriangleElem::stream(std::ostream& os) const
{
	os << "<polygon "
	   << "points=\"";
	for(gml::dvec3 p : p_)
	{
		os << p[0] << "," << p[1] << " ";
	}
	os << "\" ";
	os << "style=\"fill: " << toColor(color_) << ";\" />\n";
}

gml::dvec3 svg_writer_t::project(const gml::dvec3& p) const
{
	auto temp = gml::project(p, viewProjMatrix_, viewportOrigin_, viewportSize_);
	temp[1] = size_[1] - temp[1];
	return temp;
}

gml::dvec3 svg_writer_t::normalToColor(const gml::dvec3& normal) const
{
	double d = gml::dot(normal, lightDir_);
	d = gml::clamp(d, 0.0, 1.0);
	d = 0.1 + 0.8 * d;
	return {d, d, d};
}

svg_writer_t::svg_writer_t(int width, int height)
	: size_{width, height}
	, viewMatrix_{1.0}
	, projMatrix_{1.0}
	, viewProjMatrix_{1.0}
	, viewportOrigin_{0, 0}
	, viewportSize_{width, height}
	, lightDir_{1.0, 2.0, 3.0}
	, cullface_{true}
	, elems_{}
{
	lightDir_ = normalize(lightDir_);
}

void svg_writer_t::modelView(const gml::dmat4& matrix)
{
	viewMatrix_ = matrix;
	viewProjMatrix_ = projMatrix_ * viewMatrix_;
}

void svg_writer_t::perspective(double fovy, double aspect, double zNear, double zFar)
{
	projMatrix_ = gml::perspective(fovy, aspect, zNear, zFar);
	viewProjMatrix_ = projMatrix_ * viewMatrix_;
}

void svg_writer_t::ortho(double left, double right, double bottom, double top)
{
	projMatrix_ = gml::ortho2D(left, right, bottom, top);
	viewProjMatrix_ = projMatrix_ * viewMatrix_;
}

void svg_writer_t::viewport(int x, int y, int width, int height)
{
	viewportOrigin_ = gml::ivec2{x, y};
	viewportSize_ = gml::ivec2{width, height};
}

void svg_writer_t::cullface(bool cullface)
{
	cullface_ = cullface;
}

void svg_writer_t::writePoint(const gml::dvec3& p, const gml::dvec3& color)
{
	elems_.push_back(std::unique_ptr<BaseElem>{new VertexElem{project(p), color}});
}

void svg_writer_t::writeLine(const gml::dvec3& p1, const gml::dvec3& p2, const gml::dvec3& color)
{
	if(p1 == p2)
		return;
	elems_.push_back(std::unique_ptr<BaseElem>{new LineElem{project(p1), project(p2), color}});
}

void svg_writer_t::writeTriangle(const gml::dvec3& p1, const gml::dvec3& p2, const gml::dvec3& p3,
								 const gml::dvec3& color)
{
	if(p1 == p2 || p2 == p3 || p1 == p3)
		return;

	auto pp1 = project(p1);
	auto pp2 = project(p2);
	auto pp3 = project(p3);

	if(cullface_ && gml::normal(pp1, pp2, pp3)[2] > 0.0)
		return;

	elems_.push_back(std::unique_ptr<BaseElem>{new TriangleElem(pp1, pp2, pp3, color)});
}

void svg_writer_t::writeTriangle(const gml::dvec3& p1, const gml::dvec3& p2, const gml::dvec3& p3)
{
	writeTriangle(p1, p2, p3, normalToColor(gml::normal(p1, p2, p3)));
}

std::string svg_writer_t::str() const
{
	std::stringstream ss;

	ss << "<svg "
	   << "width=\"" << size_[0] << "\" height=\"" << size_[1] << "\" "
	   << "version=\"1.1\" "
	   << "xmlns=\"http://www.w3.org/2000/svg\""
	   << ">\n";

	ss << "<rect "
	   << "width=\"" << size_[0] << "\" height=\"" << size_[1] << "\" "
	   << "style=\"fill:white\""
	   << "/>\n";

	std::stable_sort(elems_.begin(), elems_.end(),
					 [](const std::unique_ptr<BaseElem>& e1, const std::unique_ptr<BaseElem>& e2) {
						 return e1->z_ > e2->z_;
					 });

	for(const auto& elem : elems_)
		elem->stream(ss);

	ss << "</svg>\n";

	return ss.str();
}
