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
		/*����㼯������洢˳��Ϊx,y,z*/
		bool LoadPoints(const std::vector<std::vector<float> >& pts);
		/*��ʾ����*/
		void show();
	
	private:
		vtkSmartPointer<vtkPoints> m_points;
		float m_low_z; // minimal z-value
		float m_high_z; // maximal z-value
	};
}


#endif
