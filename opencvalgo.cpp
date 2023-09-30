#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main() {
    // Устанавливаем локаль для поддержки русского языка
    setlocale(LC_ALL, "Russian");

    std::cout << "Выберите изображение для обработки:" << std::endl;
    std::cout << "1. j1.jpg" << std::endl;
    std::cout << "2. j2.jpg" << std::endl;
    std::cout << "3. j3.jpg" << std::endl;
    std::cout << "4. j4.jpg" << std::endl;
    std::cout << "5. Загрузить свой файл (укажите полный путь)" << std::endl;

    int choice;
    std::cin >> choice;

    std::string imagePath;
    if (choice >= 1 && choice <= 4) {
        // Выбрано одно из предопределенных изображений
        imagePath = "j" + std::to_string(choice) + ".jpg";
    }
    else if (choice == 5) {
        // Пользователь указывает собственный путь к файлу
        std::cin.ignore(); // Очистить буфер ввода
        std::cout << "Введите полный путь к файлу изображения:" << std::endl;
        std::getline(std::cin, imagePath);
    }
    else {
        std::cout << "Выбор некорректен. Выход." << std::endl;
        return -1;
    }

    // Загрузка изображения с диска
    cv::Mat image = cv::imread("j1.jpg");

    // Проверка, удалось ли загрузить изображение
    if (image.empty()) {
        std::cout << "Не удалось загрузить изображение." << std::endl;
        return -1;
    }

    // Перевод изображения в оттенки серого
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    // Применение порогового фильтра для выделения контуров
    cv::Mat thresholdImage;
    cv::threshold(grayImage, thresholdImage, 128, 255, cv::THRESH_BINARY);

    // Поиск контуров на изображении
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresholdImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Отфильтровать контуры спичечных коробков
    std::vector<cv::RotatedRect> matchBoxes;
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 500) {  // Минимальная площадь для контура
            cv::RotatedRect box = cv::minAreaRect(contour);
            double aspectRatio = std::max(box.size.width / box.size.height, box.size.height / box.size.width);

            if (aspectRatio >= 1.0 && aspectRatio <= 5.0) {  // Соотношение сторон
                matchBoxes.push_back(box);
            }
        }
    }

    // Наложение рамок на спичечные коробки
    cv::Scalar color(0, 0, 255); // Красный цвет для обводки
    int thickness = 2; // Толщина линии обводки
    for (const auto& box : matchBoxes) {
        cv::Point2f vertices[4];
        box.points(vertices);

        for (int i = 0; i < 4; ++i) {
            cv::line(image, vertices[i], vertices[(i + 1) % 4], color, thickness);
        }
    }

    // Сохранение обработанного изображения в файл
    cv::imwrite("result.jpg", image);

    std::cout << "Обработанное изображение сохранено в файле 'result.jpg'" << std::endl;

    return 0;
}