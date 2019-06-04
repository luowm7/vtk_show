#ifndef _POINTCLOUDRENDER_H
#define _POINTCLOUDRENDER_H

#include <vtkPoints.h>
#include <vtkSmartPointer.h>


namespace bz
{
	class PointCloudRender
	{
	public:
		PointCloudRender();
		~PointCloudRender() = default;
		/*载入点集，坐标存储顺序为x,y,z*/
		bool LoadPoints(const std::vector<std::vector<float> >& pts);
		/*显示点云*/
		void show();
	
	private:
		vtkSmartPointer<vtkPoints> m_points;
		float m_low_z; // minimal z-value
		float m_high_z; // maximal z-value
	};
}


#endif
