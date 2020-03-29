#include <chrono>

#ifndef REPORTCARD_H
#define REPORTCARD_H
namespace Component {

	using namespace std::chrono;

	class ReportCard {



	private:

		high_resolution_clock::time_point pickup_time;
		high_resolution_clock::time_point dropoff_time;


		float S_grade_time;				//sets the time to beat to get an A grade, B grade etc.
		float A_grade_time;				//in SECONDS
		float B_grade_time;
		float C_grade_time;
		float F_grade_time;



		char createFinalReport();



	public:

		ReportCard();
		ReportCard(float S_time, float A_time, float B_time, float C_time, float F_time);

		void startReportCardTimer();			//call when passenger is picked up
		void endReportCardTimer();				//call when passenger is dropped off
		void setReportCardGradeTimes(float S_time, float A_time, float B_time, float C_time, float F_time);




	};

}

#endif // !REPORTCARD_H

