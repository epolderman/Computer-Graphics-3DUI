#include "Widget.h"
#include <QtGui\qvboxlayout>
#include "myGL.h"
#include "DebugSlider.h"


Widget::Widget()
{
	QVBoxLayout * mainLayout;
	setLayout(mainLayout = new QVBoxLayout);
	QVBoxLayout * controlsLayout;
	mainLayout->addLayout(controlsLayout = new QVBoxLayout);
	mainLayout->addWidget(myGLwindow = new myGL(&myModel));

	QHBoxLayout * lightPositionLayout;
	//first row
	controlsLayout->addLayout(lightPositionLayout = new QHBoxLayout);
	lightPositionLayout->addWidget(lightSliderX = new DebugSlider);
	lightPositionLayout->addWidget(lightSliderY = new DebugSlider);
	lightPositionLayout->addWidget(lightSliderZ = new DebugSlider);

	connect(lightSliderX, SIGNAL(valueChanged(float)), this, SLOT(sliderValueChanged()));
	connect(lightSliderY, SIGNAL(valueChanged(float)), this, SLOT(sliderValueChanged()));
	connect(lightSliderZ, SIGNAL(valueChanged(float)), this, SLOT(sliderValueChanged()));

	lightSliderX->setValue(-2);
	lightSliderY->setValue(15);
	lightSliderZ->setValue(-1);
}
void Widget::sliderValueChanged()
{
	myModel.LightPosition.x = lightSliderX->value();
	myModel.LightPosition.y = lightSliderY->value();
	myModel.LightPosition.z = lightSliderZ->value();
	myGLwindow->repaint();
}


