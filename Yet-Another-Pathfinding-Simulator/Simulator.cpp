#include "stdafx.h"
#include "Simulator.h"

using namespace yaps;

Simulator::Simulator(Settings &settingsRef) : riverBottom(settingsRef.MAP_HEIGHT, settingsRef.MAP_WIDTH), dataBuffer(settingsRef.MAP_HEIGHT, settingsRef.MAP_WIDTH),
        inputCollector(dataBuffer, settingsRef), approximationEngine(dataBuffer, settingsRef), fuzzyControlSystem(settingsRef), settings(settingsRef), graph(dataBuffer) {
    boatPosition.set(settingsRef.MAP_WIDTH / 2, settings.BOAT_LENGTH);
}

Simulator::~Simulator() { }

bool Simulator::initialize(std::string filePath) {
    // If opening file fails
    if (!inputCollector.openFile(filePath))
        return false;
    // On simulator initialization, load data and approximate it
    inputCollector.loadData();
    approximationEngine.approximate();
    //Find the path using A*
    graph.findPath();
    index = 0;
    graph.getPath(refToPath);

    // Next copy it to the buffer
    riverBottom.copy(dataBuffer);
    settings.setMaxDepth(settings.getMaxBufferDepth());
    // Then load new data, approximate it and keep in the buffer
    inputCollector.loadData();
    approximationEngine.approximate();
    return true;
}

Coordinates Simulator::AStarNextPoint()
{
    if (refToPath.size() > index)
    {
        int prev_index = index;
        index += settings.STEP;
        return  refToPath[prev_index];
    }
    else return Coordinates(-1, -1);
}

bool Simulator::run() {
    if (boatPosition.y >= settings.MAP_HEIGHT) {
        if (dataBuffer.getHeight() == 0)
            return false;
        //Update A* path
        graph.findPath();
        graph.getPath(refToPath);
        index = 0;

        riverBottom.copy(dataBuffer);
        settings.setMaxDepth(settings.getMaxBufferDepth());

        inputCollector.loadData();
        approximationEngine.approximate();
        settings.setimageHeight(settings.getimageHeight() - settings.MAP_HEIGHT);

        boatPosition.set(boatPosition.x, settings.BOAT_LENGTH);
        return true;
    }
    sf::Clock clock;
    updateAdjecentPoints();
    fuzzyControlSystem.run(frontPoints, leftPoints, rightPoints, minValue, maxValue);
    moveBoat(fuzzyControlSystem.getAngle(), fuzzyControlSystem.getSpeed());

#ifdef DEBUG
    std::cout << "---------------------------\n" << "Simulator\n\n"
        << "Boat Position: [" << boatPosition.x << "][" << boatPosition.y << "]\n"
        << "Current boat angle: " << boatAngle << '\n'
        << "Current boat speed: " << boatSpeed << '\n'
        << "Simulation time: " << clock.getElapsedTime().asSeconds()
        << " s\n---------------------------\n";
#endif
#ifdef VERBOSE_DEBUG
    printCurrentData();
#endif

    return true;
}

void Simulator::moveBoat(float angle, float speed) {
    // TODO: Use speed in calculations.
    boatAngle += angle;
    boatAngle = boatAngle > settings.MAX_ANGLE ? settings.MAX_ANGLE : boatAngle < -settings.MAX_ANGLE ? -settings.MAX_ANGLE : boatAngle;
    boatSpeed = speed <= settings.MAX_SPEED ? speed : settings.MAX_SPEED;
    VECTOR2 boat((float)boatPosition.x, (float)boatPosition.y);
    VECTOR2 relative(0.f, (float)settings.STEP);
    relative %= -boatAngle;
    relative += boat;
    boatPosition.x = round(relative.x);
    boatPosition.y = round(relative.y);
}

void Simulator::updateAdjecentPoints() {
    VECTOR2 boat((float)boatPosition.x, (float)boatPosition.y);
    VECTOR2 front, left, right;
    int halfWidth = settings.BOAT_WIDTH / 2;
    int length = settings.BOAT_LENGTH;

    frontPoints.clear();
    leftPoints.clear();
    rightPoints.clear();

    // Initialize min and max values, which will be updated in addPointToVector method
    maxValue = FLT_MIN;
    minValue = FLT_MAX;

    for (int j = settings.PROXIMITY; j > 0; j--) {
        // Iterate along the width of the boat
        for (int i = -halfWidth; i <= halfWidth; i++) {
            front = VECTOR2((float)i, (float)j);
            // Rotate by the given angle
            // Positive angle rotates vector counterclockwise so we negate it to keep rule:
            // positive angle rotates the boat to the right
            front %= -boatAngle;
            // Move to the position relative to boat
            front += boat;
            // Push value at this coordinates to front points vector
            addPointToVector(frontPoints, round(front.x), round(front.y));
        }
        // Iterate along the length
        for (int i = length - 1; i >= 0; i--) {
            left = VECTOR2((float)(-halfWidth - j), (float)(i));
            right = VECTOR2((float)(halfWidth + j), (float)(i));
            left %= -boatAngle;
            right %= -boatAngle;
            left += boat;
            right += boat;
            addPointToVector(leftPoints, round(left.x), round(left.y));
            addPointToVector(rightPoints, round(right.x), round(right.y));
        }
    }
}

void Simulator::printCurrentData() {
    std::cout << "River Bottom Data: \n";
    for (int yi = riverBottom.getHeight() - 1; yi >= 0; yi--) {
        for (int xi = riverBottom.getWidth() - 1; xi >= 0; xi--) {
            if (yi == (int)boatPosition.y && xi == (int)boatPosition.x)
                std::cout << 'X' << "\t";
            else
                std::cout << std::fixed << std::setprecision(2) << riverBottom[yi][xi] << "\t";
        }
        std::cout << '\n';
    }
}

void Simulator::reset()
{
    initialize(inputCollector.getPath());
    boatPosition.set(settings.MAP_WIDTH / 2,  settings.BOAT_LENGTH);

}
