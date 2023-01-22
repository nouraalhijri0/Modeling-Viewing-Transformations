#pragma once
#include <iostream>

class Color{
public:
	double red;
	double green;
	double blue;	
	Color() : red(0),green(0),blue(0){}
	Color(double c) : red(c), green(c), blue(c) {}
	Color(double r, double g, double b) : red(r), green(g), blue(b){}
	Color(const Color &_color) : red(_color.red), green(_color.green), blue(_color.blue) {}

	Color operator* (double a){
	    return Color(red * a, green * a, blue * a);
	}
	
	Color operator+ (Color a){
	    return Color(red + a.red, green + a.green, blue + a.blue);
	}

	Color operator* (Color a) {
		return Color(red * a.red, green * a.green, blue * a.blue);
	}
};

std::ostream& operator<< (std::ostream& os, const Color& c)
{
	os << "Color: " << c.red << ", " << c.green << ", " << c.blue << "\n";
	return os;
}
