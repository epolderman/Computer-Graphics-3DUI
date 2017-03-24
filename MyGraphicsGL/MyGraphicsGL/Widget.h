#ifndef WIDGET_H
#define WIDGET_H
#include <Qt\qwidget.h>
#include "ModelData.h"
class DebugSlider;
class myGL;

class Widget : public QWidget
{
	Q_OBJECT;

private slots:
	void sliderValueChanged();
private:
	DebugSlider* lightSliderX;
	DebugSlider* lightSliderY;
	DebugSlider* lightSliderZ;
	myGL* myGLwindow;
	ModelData myModel;
public:
	Widget();
};

#endif
