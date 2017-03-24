#include <Qt\qapplication.h>
#include "myGL.h"
#include "Widget.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	
	Widget theWid;
	theWid.show();

	//myGL theWindow;
	//theWindow.show();




	return app.exec();
}