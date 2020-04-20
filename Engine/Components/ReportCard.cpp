
#include "ReportCard.h"
#include <iostream>


Component::ReportCard::ReportCard() {

    
    report_bill_mesh =
        getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
            "Assets/Meshes/billboard_quad.obj");
}

Component::ReportCard::ReportCard(float S, float A, float B, float C, float F) {

    
    report_bill_mesh =
        getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
            "Assets/Meshes/billboard_quad.obj");

  S_grade_time = S;
  A_grade_time = A;
  B_grade_time = B;
  C_grade_time = C;
  F_grade_time = F;
}

void Component::ReportCard::setReportCardGradeTimes(float S, float A, float B,
                                                    float C, float F) {
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

  createFinalReport();
  displayReportCard();
}



 void Component::ReportCard::createFinalReport() {
  duration delta = dropoff_time - pickup_time;

  delta = delta / 1000000000; // delta is now in seconds

  std::cout << "Time taken to drop off passenger: " << delta.count()
            << std::endl;
  ;

  if (delta.count() <= S_grade_time) {
    grade = 'S';


  } else if (delta.count() < A_grade_time) {
    grade = 'A';


  } else if (delta.count() < B_grade_time) {
    grade = 'B';


  } else if (delta.count() < C_grade_time) {
    grade = 'C';


  }

  else {
    grade = 'F';


  }
}

void Component::ReportCard::displayReportCard() {



  auto filename = "";

  if (grade == 'S')
    filename = "Assets/Textures/report_card_s.png";

  else if (grade == 'A')
    filename = "Assets/Textures/report_card_a.png";

  else if (grade == 'B')
    filename = "Assets/Textures/report_card_b.png";

  else if (grade == 'C')
    filename = "Assets/Textures/report_card_c.png";

  else if (grade == 'F')
    filename = "Assets/Textures/report_card_f.png";



  std::cout << "createReportCard" << std::endl;
  report_card_sprite = getEngine()->createComponent<Component::Billboard>(
      getEngine()->createComponent<Component::Texture>(filename));

  report_card_sprite->plot = BoxModel{0, -0.5, 256, 256};
  report_card_sprite->dst = RelativeAnchor{0, 1};
  report_card_sprite->src = { 0,1 };

  report_card_disp_start = std::chrono::steady_clock::now();
}

void Component::ReportCard::destroyReportCard() {
  std::cout << "destroyReportCard" << std::endl;
  
  getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(
      report_card_sprite.get());
}