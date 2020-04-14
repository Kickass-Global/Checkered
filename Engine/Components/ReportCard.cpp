
#include "ReportCard.h"
#include <iostream>

Component::ReportCard::ReportCard(float S, float A, float B, float C, float F) {

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
}

bool Component::ReportCard::reportCardTimeOut() {

  report_card_disp_end = steady_clock::now();

  duration delta = report_card_disp_end - report_card_disp_start;
  delta = delta / 1000000000;

  if (delta.count() >= 5) {
    return true;
  } else {
    return false;
  }
}

char Component::ReportCard::createFinalReport() {
  duration delta = dropoff_time - pickup_time;

  delta = delta / 1000000000; // delta is now in seconds

  std::cout << "Time taken to drop off passenger: " << delta.count()
            << std::endl;
  ;

  if (delta.count() <= S_grade_time) {
    grade = 'S';

    return 'S';
  } else if (delta.count() < A_grade_time) {
    grade = 'A';

    return 'A';
  } else if (delta.count() < B_grade_time) {
    grade = 'B';

    return 'B';
  } else if (delta.count() < C_grade_time) {
    grade = 'C';

    return 'C';
  }

  else {
    grade = 'F';

    return 'F';
  }
}

void Component::ReportCard::displayReportCard(char grade) {

  isDisplayed = true;

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
    filename = "Assets/Textures/report_card_c.png";

  auto report_bill_mesh =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/billboard_quad.obj");

  std::cout << "createReportCard" << std::endl;
  report_card_sprite = getEngine()->createComponent<Component::Billboard>(
      getEngine()->createComponent<Component::Texture>(filename));

  report_card_sprite->plot = BoxModel{0, 0, 256, 256};
  report_card_sprite->dst = RelativeAnchor{0, 0};

  report_card_disp_start = std::chrono::steady_clock::now();
}

void Component::ReportCard::destroyReportCard() {
  std::cout << "destroyReportCard" << std::endl;
  isDisplayed = false;
  getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(
      report_card_sprite.get());
}