
#include "ReportCard.h"

Component::ReportCard::ReportCard(float S, float A, float B, float C, float F) {

	S_grade_time = S;
	A_grade_time = A;
	B_grade_time = B;
	C_grade_time = C;
	F_grade_time = F;


}

void Component::ReportCard::setReportCardGradeTimes(float S, float A, float B, float C, float F) {
	S_grade_time = S;
	A_grade_time = A;
	B_grade_time = B;
	C_grade_time = C;
	F_grade_time = F;
}

void Component::ReportCard::startReportCardTimer() {
	
	pickup_time = std::chrono::high_resolution_clock::now();
}

void Component::ReportCard::endReportCardTimer() {
	dropoff_time = std::chrono::high_resolution_clock::now();
}

char Component::ReportCard::createFinalReport() {
	typedef duration<float> floatMilliseconds;
	std::chrono::duration<float, milliseconds> delta = dropoff_time - pickup_time;

	delta = delta / 1000;		//delta is now in seconds
	
	if (delta.count() <= S_grade_time) return 'S';
	else if (delta.count() < A_grade_time) return 'A';
	else if (delta.count() < B_grade_time) return 'B';
	else if (delta.count() < C_grade_time) return 'C';
	else return 'F';




}