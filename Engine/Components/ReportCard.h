#include <chrono>
#include <ComponentBase.h>
#include "Pipeline/Library.h"
#include "Pipeline/MeshLoader.h"
#include "Billboard.h"

#ifndef REPORTCARD_H
#define REPORTCARD_H
namespace Component {

	using namespace std::chrono;

	class ReportCard : public ComponentBase {



	private:


		steady_clock::time_point pickup_time;
		steady_clock::time_point dropoff_time;
		steady_clock::time_point report_card_disp_start;
		steady_clock::time_point report_card_disp_end;

		

	
		/*
		std::shared_ptr<Component::Mesh> report_bill_mesh = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/billboard_quad.obj"
			);
			*/
		std::shared_ptr<Component::Billboard> report_card_sprite;

		float S_grade_time;				//sets the time to beat to get an A grade, B grade etc.
		float A_grade_time;				//in SECONDS
		float B_grade_time;
		float C_grade_time;
		float F_grade_time;



		



	public:
		bool isDisplayed = false;
		char grade;

		ReportCard() {}
		ReportCard(float S_time, float A_time, float B_time, float C_time, float F_time);

		void startReportCardTimer();			//call when passenger is picked up
		void endReportCardTimer();				//call when passenger is dropped off
		void setReportCardGradeTimes(float S_time, float A_time, float B_time, float C_time, float F_time);
		void displayReportCard(char);
		void destroyReportCard();
		bool reportCardTimeOut();
		char createFinalReport();



	};

}

#endif // !REPORTCARD_H

