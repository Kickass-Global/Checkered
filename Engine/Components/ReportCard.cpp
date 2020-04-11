
#include "ReportCard.h"
#include <iostream>

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
	
	pickup_time = std::chrono::steady_clock::now();
}

void Component::ReportCard::endReportCardTimer() {
	dropoff_time = std::chrono::steady_clock::now();
}

bool Component::ReportCard::reportCardTimeOut() {

	report_card_disp_end = steady_clock::now();

	duration delta = report_card_disp_end - report_card_disp_start;
	delta = delta / 1000000000;
	std::cout <<"delta count: " <<delta.count() << std::endl;
	if (delta.count() >= 5) {
		return true;
	}
	else {
		return false;
	}
}

char Component::ReportCard::createFinalReport() {
	duration delta = dropoff_time - pickup_time;

	delta = delta / 1000000000;		//delta is now in seconds
	
	if (delta.count() <= S_grade_time) {
		grade = 'S';
		return 'S';
	}
	else if (delta.count() < A_grade_time) {
		grade = 'A';
		return 'A';
	}
	else if (delta.count() < B_grade_time) {
		grade = 'B';
		return 'B';
	}
	else if (delta.count() < C_grade_time)
	{
		grade = 'C';
		return 'C';
	}

	else {
		grade = 'F';
		return 'F';
	
	}




}

void Component::ReportCard::displayReportCard(char grade) {

	std::cout << "show report card" << std::endl;
	auto filename = "";

	switch (grade)
	{
	case 'S':
		filename = "Assets/Textures/report_card_s.png";

	case 'A':
		filename = "Assets/Textures/report_card_s.png";

	case 'B':
		filename = "Assets/Textures/report_card_s.png";

	case 'C':
		filename = "Assets/Textures/report_card_s.png";

	case 'F':
		filename = "Assets/Textures/report_card_s.png";


	default:
		std::cout << "No grade assigned to report card"<<std::endl;
		break;
	}

	


	auto report_bill_mesh = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
		"Assets/Meshes/billboard_quad.obj"
		);
	report_bill_mesh->getStore().emplaceComponent<Component::Billboard>();

	report_card_sprite = getEngine()->createComponent<Component::Billboard>();
	report_card_sprite->plot = BoxModel{ 0,0,256,256 };
	report_card_sprite->dst = RelativeAnchor{ 0,0 };
	{
		auto report_card_mat = getEngine()->createComponent<Component::Material>(
			getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
				"Assets/Programs/billboard.json"
				)
			);

		report_card_mat->textures.push_back(
			getEngine()->createComponent<Component::Texture>(
				filename
				)
		);

		report_card_sprite->mesh_instance = getEngine()->createNamedComponent<PaintedMesh>(
			"report_card_mesh_instance", report_bill_mesh, report_card_mat);
		

	}
	
	isDisplayed = true;
	report_card_disp_start = std::chrono::steady_clock::now();

}

void Component::ReportCard::destroyReportCard() {
	std::cout << "destroyReportCard" << std::endl;
	isDisplayed = false;
	getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(report_card_sprite.get());
}