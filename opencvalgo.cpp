#include <opencv2/opencv.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <iostream>
#include <vector>

namespace bg = boost::geometry;

typedef bg::model::point<int, 2, bg::cs::cartesian> Point;
typedef bg::model::polygon<Point> Polygon;

int main() {
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
    cv::Mat image = cv::imread(imagePath);

    // Проверка, удалось ли загрузить изображение
    if (image.empty()) {
        std::cout << "Не удалось загрузить изображение." << std::endl;
        return -1;
    }

    // Преобразование изображения в оттенки серого
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    // Применение порогового фильтра для выделения контуров
    cv::Mat thresholdImage;
    cv::threshold(grayImage, thresholdImage, 128, 255, cv::THRESH_BINARY);

    // Поиск контуров на изображении
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresholdImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Отфильтровать контуры спичечных коробков
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 500) {  // Минимальная площадь для контура
            double aspectRatio = static_cast<double>(cv::boundingRect(contour).width) / cv::boundingRect(contour).height;
            if (aspectRatio >= 1.0 && aspectRatio <= 5.0) {  // Соотношение сторон (горизонтальные объекты)
                // Нарисовать рамку вокруг спичечного коробка
                cv::Rect rect = cv::boundingRect(contour);
                cv::rectangle(image, rect, cv::Scalar(0, 0, 255), 2);
            }
        }
    }

    // Сохранение обработанного изображения в файл
    cv::imwrite("result.jpg", image);

    std::cout << "Обработанное изображение сохранено в файле 'result.jpg'" << std::endl;

    return 0;
}
