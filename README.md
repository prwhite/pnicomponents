# PNI Components

## Overview

This is a collection of components for the esp-idf build environment for ESP32 application development.

References:
* [esp-idf build environment](http://esp-idf.readthedocs.io/en/latest/api-guides/build-system.html#)
* [esp-idf component guidelines](http://esp-idf.readthedocs.io/en/latest/api-guides/build-system.html#component-makefiles)

The included components are generally re-usable and have limited coupling, allowing applications to easily use a subset of the compnoents.

Functionality covered by the components (**pretty much all of these are TODOs**):
* Processing:
    * `Task`: A simple wrapper around FreeRTOS `xTaskCreate` and related functions \[[more](http://www.freertos.org/a00125.html)\].  Also included:
        * `TaskLambda`: A task that takes a C++11 lambda for the task callback rather than requiring the developer to derive a class and override the virtual task method.
    * `Queue`: A thread safe FIFO.  Thread safe only for push and pop.
    * `LambdaQueue`:  A queue for lambdas to be passed safely from one task to another.
    * `Actor`: A task with an associated lambda queue.
    * `Dispatcher`: To send/receive process-wide notifications.
* Presentation:
    * `Color`: For manipulating colors in RGB and HSV space.  Includes lerp functions in both color spaces which is handy for LED animations.
    * `ArrayResampler`: For up and downsampling an array of data.  Handy for scaling 1D image data for display on different size LED strips.  Should probably make a 2D array resampler and then get 1D functionality _for free_.
    * `Gauge`: Classes to show data in various forms.  Targeted for monochrome displays (i.e., OLEDs such as SSD1306), so not too fancy.
        * `GaugeLinear`: Bar and line graphs for 1D data.
        * `GaugeRadial`: Radial graph (a la speedometer or tachometer).

## Recommended Usage

Given an application project for the esp-idf, do the following:

* Add a submodule (assuming you are using git):
    * Change directory to the root of the project
    * `mkdir 3p` # an example name for 3rd party code
    * `cd 3p`
    * `git submodule add <repo url> pnicomponents`
        * There's always a chance you might need to do a `git submodule sync` or `init` depending on your git version.
* Symbolic link desired components into build system:
    * Change directory to the root of the project
    * `mkdir components` # name must be 'components'
    * `cd components`
    * `ln -s ../3p/pnicomponents/components/<some-component> <some-component>`

When you do your next build, the `<some-component>` component will automatically be built and linked into your project.  The headers from the component will automatically be available to your application code and other components.

## Troubleshooting

* Issue: Build fails
    * Probably due to component dependencies.  While there is minimal coupling between components in this project, some do exist and not adding a symbolic link for a dependeny in the `components` directory can cause this kind of failure.

## About PNI

The PNI prefix is an arbitrary thing to give all the classes a namespace to avoid collisions with other libraries and system symbol names.  Don't ask what it stands for.


