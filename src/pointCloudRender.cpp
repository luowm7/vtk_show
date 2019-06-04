#include "pointCloudRender.h"

#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
#include <vtkDebugLeaks.h>
#include <vtkPolyData.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkElevationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkAutoInit.h>
#include <vtkPointPicker.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkTextActor.h>
#include <vtkObjectFactory.h>
#include <vtkLabeledDataMapper.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>


VTK_MODULE_INIT(vtkRenderingOpenGL2); //非cmake生成的工程需添加
VTK_MODULE_INIT(vtkRenderingFreeType);

class PointPickerInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	PointPickerInteractorStyle(){};
	~PointPickerInteractorStyle(){};

	static PointPickerInteractorStyle* New();

	virtual void OnLeftButtonDown()
	{
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

		int *clickPos = this->GetInteractor()->GetEventPosition();
		vtkSmartPointer<vtkPointPicker> pointPicker = vtkSmartPointer<vtkPointPicker>::New();

		pointPicker->Pick(clickPos[0], clickPos[1], 0, this->GetCurrentRenderer());
		double *pickCoordinate = pointPicker->GetPickPosition();
		std::string str = "X: " + std::to_string(pickCoordinate[0]) + " Y: " + std::to_string(pickCoordinate[1]) + " Z: " + std::to_string(pickCoordinate[2]);
		//std::cout << str << std::endl;

		textActor->SetInput(str.c_str());

	}
	void unitizeTextActor(vtkSmartPointer<vtkTextActor> textActor)
	{
		this->textActor = textActor;
		//this->renderer = renderer;
	}
private:
	vtkSmartPointer<vtkTextActor> textActor;
	//vtkRenderer* renderer;
};
vtkStandardNewMacro(PointPickerInteractorStyle);

bz::PointCloudRender::PointCloudRender() :m_low_z(FLT_MAX), m_high_z(FLT_MIN)
{
	m_points = vtkSmartPointer<vtkPoints>::New();
}

bool bz::PointCloudRender::LoadPoints(const std::vector<std::vector<float> >& pts)
{
	//m_points->Allocate(0);
	//m_points->Reset();
	//m_points = vtkSmartPointer<vtkPoints>::New();
	m_points->Resize(0);
	if (pts.empty())  return false;

	m_low_z = std::numeric_limits<float>::max();
	m_high_z = std::numeric_limits<float>::lowest();

	for (size_t i = 0; i < pts.size(); i++)
	{
		if (pts[i].size() != 3) return false;

		float x = pts[i][0];
		float y = pts[i][1];
		float z = pts[i][2];
		if (z < m_low_z)
			m_low_z = z;
		else if (z > m_high_z)
			m_high_z = z;

		m_points->InsertNextPoint(x, y, z);
	}

	return true;
}

void bz::PointCloudRender::show()
{
	std::cout << "Points :" << m_points->GetNumberOfPoints() << std::endl;

	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	polydata->SetGlobalWarningDisplay(0); // close vtkOutoutWindow
	polydata->SetPoints(m_points);

	vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
	glyphFilter->SetInputData(polydata);
	glyphFilter->Update();

	vtkSmartPointer<vtkElevationFilter> elevationFilter = vtkSmartPointer<vtkElevationFilter>::New();
	elevationFilter->SetInputConnection(glyphFilter->GetOutputPort());
	elevationFilter->SetLowPoint(0, 0, m_low_z);
	elevationFilter->SetHighPoint(0, 0, m_high_z);

	vtkSmartPointer<vtkPolyDataMapper> dataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	dataMapper->SetInputConnection(elevationFilter->GetOutputPort());

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(dataMapper);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);
	renderer->SetBackground(.0, .0, .0);

	vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
	textActor->SetInput("");
	vtkSmartPointer<vtkTextProperty> tprop = textActor->GetTextProperty();
	tprop->SetFontFamilyToArial();
	tprop->SetFontSize(16);
	tprop->BoldOn();
	tprop->SetColor(0, 1, 0);
	textActor->SetDisplayPosition(30, 30);
	renderer->AddActor2D(textActor);

	//添加坐标轴
	//vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
	//axesActor->SetTotalLength(10, 10, 10);
	//axesActor->SetCylinderRadius(0.500 * axesActor->GetCylinderRadius());
	//axesActor->SetConeRadius(1.025 * axesActor->GetConeRadius());
	//axesActor->SetSphereRadius(1.500 * axesActor->GetSphereRadius());
	/*vtkTextProperty* Xprop = axesActor->GetXAxisCaptionActor2D()->GetCaptionTextProperty();
	Xprop->ItalicOn();
	Xprop->ShadowOff();
	Xprop->SetFontFamilyToTimes();
	Xprop->SetFontSize(20);*/
	//renderer->AddActor(axesActor);

	// add color bar
	vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
	vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
	hueLut->SetTableRange(m_low_z, m_high_z);
	hueLut->SetHueRange(0, 1);
	hueLut->SetSaturationRange(1, 1);
	hueLut->SetValueRange(1, 1);
	hueLut->Build();
	scalarBar->SetLookupTable(hueLut);
	scalarBar->SetWidth(0.1);
	scalarBar->SetNumberOfLabels(7);
	renderer->AddActor2D(scalarBar);

	vtkSmartPointer<vtkRenderWindow> renderwind = vtkSmartPointer<vtkRenderWindow>::New();
	renderwind->AddRenderer(renderer);

	vtkSmartPointer<PointPickerInteractorStyle> style = vtkSmartPointer<PointPickerInteractorStyle>::New();
	style->unitizeTextActor(textActor);

	vtkSmartPointer<vtkRenderWindowInteractor> renderwindIt = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderwindIt->SetRenderWindow(renderwind);
	renderwindIt->SetInteractorStyle(style);

	renderwind->Render();
	//renderwindIt->Enable();
	renderwindIt->Start();
	
	//renderwindIt->Disable();
}
