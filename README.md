# PNI Components

## Overview

This is a collection of components for the esp-idf build environment for ESP32 application development.

References:
* [esp-idf build environment](http://esp-idf.readthedocs.io/en/latest/api-guides/build-system.html#)
* [esp-idf component guidelines](http://esp-idf.readthedocs.io/en/latest/api-guides/build-system.html#component-makefiles)

The included components are generally re-usable and have limited coupling, allowing applications to easily use a subset of the compnoents.

Functionality covered by the components (some of these are TODOs):
* Processing:
    * `Task`: A simple wrapper around FreeRTOS `xTaskCreate` and related functions \[[more](http://www.freertos.org/a00125.html)\].  Also included:
        * `TaskLambda`: A task that takes a C++11 lambda for the task callback rather than requiring the developer to derive a class and override the virtual task method.
    * `Queue`: A thread safe FIFO.  Thread safe only for push and pop.
    * `LambdaQueue`:  A queue for lambdas to be passed safely from one task to another.
    * `Actor`: A task with an associated lambda queue.
* Presentation:
    * `Color`: A family of classes (or something like that) for manipulating colors in RGB and HSV space.  Includes lerp functions in both color spaces which is handy for LED animations.
    * `ArrayResampler`: A class for up and downsampling an array of data.  Handy for scaling 1D image data for display on different size LED strips.  Should probably make a 2D array resampler and then get 1D functionality _for free_.

## Recommended Usage

Given an application project for the esp-idf, do the following:

* Check out the submodule:
    * Change directory to the root of the project
    * `mkdir 3p`
    * `cd 3p`
    * `git clone ...` this project into the current working directory
* Symbolic link desired components into build system:
    * Change directory to the root of the project
    * `mkdir components`
    * `cd components`
    * `ln -s ../3p/pnicomponents/components/<some-component> <some-component>`

When you do your next build, the `<some-component>` component will automatically be built and linked into your project.  The headers from the component will automatically be available to your application code and other components.

## Troubleshooting

* Issue: Build fails
    * Probabl due to component dependencies.  While there is minimal coupling between components in this project, some do exist and not adding a symbolic link for a dependeny in the `components` directory can cause this kind of failure.

## About PNI

The PNI prefix is an arbitrary thing to give all the classes a namespace to avoid collisions with other libraries and system symbol names.  Don't ask what it stands for.


