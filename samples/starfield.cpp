#include <iostream>
using std::ostream;
using std::cout;

#include <stdlib.h>
const int xmax = 600;
const int ymax = 600;

int random_int(int min, int max)
{
  long r = rand();
  return (int)(r * (max - min) / RAND_MAX) +  min;
}

typedef enum _Shape {
  rect,
  circle,
  last_shape
} Shape;


void generate(ostream& out, int size_record)
{
  for (int i = 0; i < size_record; i++) {
#if 0
    out << " <use xlink:href='#";
    switch(random_int(0,last_shape)) {
    case rect:
      out << "rect'";
      break;
    case circle:
      out << "circle'";
      break;
    }
    out << " transform='"
	<< "translate(" 
	<< random_int(0, xmax) << " "
	<< random_int(0, ymax) << ")"
	<< " scale("
	<< random_int(1, 200) / 10.0f
	<< ")'";
    out << " style='fill:rgb("
	<< random_int(0,255) << ","
	<< random_int(0,255) << ","
	<< random_int(0,255) << ")'";
    out << "/>\n";
#else
    float scale = random_int(1, 200) / 10.0f;

    out << "<rect "
	<< "x='"
	<< random_int(0, xmax) << "' "
	<< "y='"      
	<< random_int(0, ymax) << "' "
	<< "width='"
	<< scale << "' "
	<< "height='"
	<< scale << "' ";
    out << "style='fill:rgb("
	<< random_int(0,255) << ","
	<< random_int(0,255) << ","
	<< random_int(0,255) << ")'";
    out << "/>\n";
#endif
  }
}

int main(int argc, char * argv[])
{
  int n = 1000;
  if (argc > 1) {
    n = atoi(argv[1]);
  }
  cout << "<?xml version = \"1.0\" standalone = \"no\"?>\n\
<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20000802//EN\" \n\
  \"http://www.w3.org/TR/2000/CR-SVG-20000802/DTD/svg-20000802.dtd\">\n\
<svg width=\"" << xmax << "\" height=\"" << ymax << "\">\n";
#if 0
  cout << "<defs>\n\
 <rect id=\"rect\" x=\"0\" y=\"0\" width=\"1\" height=\"1\"/>\n\
 <rect id=\"circle\" x=\"0\" y=\"0\" width=\"1\" height=\"1\"/>\n\
 <circle id=\"circle2\" cx=\"0\" cy=\"0\" r=\"1\"/>\n\
</defs>\n";
#endif
  cout << "<g style='fill-opacity:0.5;stroke:none;'>\n";
  cout << "<animateTransform attributeName=\"transform\" attributeType=\"XML\" type=\"scale\" from=\"0.1\" to=\"1.5\" begin=\"0s\" dur=\"30s\" repeatCount=\"indefinite\"/>\n";

  generate(cout, n);
  cout << "</g>\n</svg>\n";
  return 0;
}
