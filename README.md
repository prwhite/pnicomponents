# PNI Components

## Overview

This is a collection of components for the esp-idf build environment for ESP32 application development.

References:
* [esp-idf build environment](http://esp-idf.readthedocs.io/en/latest/api-guides/build-system.html#)
* [esp-idf component guidelines](http://esp-idf.readthedocs.io/en/latest/api-guides/build-system.html#component-makefiles)

The included components are generally re-usable and have limited coupling, allowing applications to easily use a subset of the compnoents.

Functionality covered by the components (**pretty much all of these are TODOs**):
* Processing:
    * `Task`: ![Done](src/label-done.png) ![Untested](src/label-untested.png) A simple wrapper around FreeRTOS `xTaskCreate` and related functions \[[more](http://www.freertos.org/a00125.html)\].  Also included:
        * `TaskLambda`: ![Done](src/label-done.png) ![Untested](src/label-untested.png) A task that takes a C++11 lambda for the task callback rather than requiring the developer to derive a class and override the virtual task method.
    * `Fft`: ![Done](src/label-done.png) ![Untested](src/label-untested.png) An embedded-optimized FFT.
        * Using fix_fft originally from [here](https://github.com/fmilburn3/FFT), but the basic implementation is all over the internet.
    * `Queue`: A thread safe FIFO.  Thread safe only for push and pop.
    * `LambdaQueue`:  A queue for lambdas to be passed safely from one task to another.
    * `Actor`: A task with an associated lambda queue.
    * `Dispatcher`: To send/receive process-wide notifications.
* Presentation:
    * `FixedPoint`: ![Done](src/label-done.png) ![Tested](src/label-tested.png) A template class that handles arbitrary precision fixed-point math.  Handy for integer-based operations on color components for driving LEDs, but has many uses beyond that in the embedded world.
    * `Color`: ![Done](src/label-done.png) ![Tested](src/label-tested.png) For manipulating colors in RGB and HSV space.  Includes lerp functions in both color spaces which is handy for LED animations.
    * `ArrayResampler`: For up and downsampling an array of data.  Handy for scaling 1D image data for display on different size LED strips.  Should probably make a 2D array resampler and then get 1D functionality _for free_.
    * `Gauge`: ![In Progress](src/label-progress.png) Classes to show data in various forms.  Targeted for monochrome displays (i.e., OLEDs such as SSD1306), so not too fancy.
        * `GaugeLinear`: Bar and line graphs for 1D data.
        * `GaugeRadial`: Radial graph (a la speedometer or tachometer).
    * `Mapper` For clut to map colors between display devices.  Can be used for keeping constant brightness while animating hue.
* Drivers:
    * Apa102: ![Done](src/label-done.png) ![Tested](src/label-tested.png) A simple set of classes to drive APA102 LEDs.  Has an abstraction to accomodate S/W and H/W SPI.  Note: Only the S/W interface is currently implemented.
    * Msgeq7: ![In Progress](src/label-progress.png) Configure and read the MSGEQ7 graphic EQ chip.
    * Max9814: ![In Progress](src/label-progress.png) Auto-gain control mic amp, used with Adafruit Electrec Mic breakout: [Adafruit Electret Microphone Amplifier](https://www.adafruit.com/product/1713).

## Recommended Usage

Given an application project for the esp-idf, do the following:

* Add a submodule (assuming you are using git):
    * Change directory to the root of the project
    * `mkdir 3p` # an example name for 3rd party code directory
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


