#include "raytracer.h"
#include "raytracescene.h"
#include "utils/intersections.h"
#include <cmath>
#include <limits>
#include "utils/light.h"
#include "utils/ray.h"
#include <iostream>
#include "pixelraytracer.h"

#include <QtConcurrent>
#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>
#include <QtCore/QObject>


RayTracer::RayTracer(Config config) :
    m_config(config)
{
}




void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {
    auto start = std::chrono::high_resolution_clock::now();

//    Node root;
//    root.box = AAB(-5,-5,-5,5,5,5);
//    root.shapes = scene.getMetaData().shapes;

//    Node* tree = KDTree::constructNode(&root,0);
//    KDTree::visualizeBinaryTree(tree);

    //

//    for(int j=0;j<scene.height();j++) {
//        for (int i=0;i<scene.width();i++) {
//            imageData[j*scene.width() + i] = renderPixel(scene,i,j);
//        }
//    }

    PixelRaytracer pRaytracer(imageData,scene);

    QList<int> inputValues;
    for (int i = 0; i < scene.width()* scene.height(); ++i) {
        inputValues.append(i);
//        RGBA out = pRaytracer(i);
    }

    QFuture<RGBA> results = QtConcurrent::mapped(inputValues,pRaytracer);
    results.waitForFinished();
    QList<RGBA> re = results.results();

    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time: " << duration.count()/1.0e6 << " seconds" << std::endl;
}



