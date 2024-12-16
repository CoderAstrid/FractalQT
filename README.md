# FractalQT

**FractalQT** is a high-performance fractal rendering application built using **Qt** and optimized with multithreading, advanced algorithms, and customizable color palettes. This project allows the generation and exploration of fractal patterns such as the Mandelbrot and Julia sets with user-configurable rendering settings.

---

## Screen Shot

![Mandelbrot](screenshots/rev01.PNG?raw=true "Mandelbrot")

![Mandelbort and Julia](screenshots/rev02-1.PNG?raw=true "Main UI 1")

![Mandelbort and Julia](screenshots/rev02.PNG?raw=true "Main UI 2")

---

## **Features**

1. **High-Performance Fractal Rendering**:

   - Multithreaded rendering using `QThreadPool` and `OpenMP`.
   - Adaptive computation for better performance on zoomed-in regions.

2. **Fractal Types Supported**:

   - Mandelbrot Set.
   - Julia Set with adjustable parameters.

3. **Dynamic Color Palettes**:

   - Predefined palettes: Grayscale, Spectral, HeatMap, YlGnBu, HeightMap.
   - Support for custom palettes loaded from `.map` files.
   - Real-time palette switching and preview.

4. **Interactive Exploration**:

   - Zoom and pan controls for fractal exploration.
   - Real-time updates on mouse movements.

5. **Customizable Rendering**:

   - Adjustable maximum iterations for higher precision.
   - Smooth coloring for gradient-based visuals.

6. **Cross-Platform Support**:

   - Works on Windows, macOS, and Linux.

---

## **Getting Started**

### **Prerequisites**

1. **Qt Framework**: Install Qt 5.14.2 or later.
2. **Compiler**:
   - MinGW (for Windows) or GCC/Clang (for macOS/Linux).
   - Ensure support for `OpenMP` for multithreading.
3. **C++ Compiler**: Support for C++11 or higher.

---

### **Building the Project**

#### **Using qmake**

1. Clone the repository:

   ```bash
   git clone https://github.com/CoderAstrid/FractalQT.git
   cd FractalQT
   ```

2. Run `qmake`:

   ```bash
   qmake FractalQT.pro
   ```

3. Build the project:

   ```bash
   make
   ```

4. Run the application:

   ```bash
   ./FractalQT
   ```

#### **Using CMake**

1. Configure and generate build files:

   ```bash
   cmake -S . -B build
   ```

2. Build the application:

   ```bash
   cmake --build build
   ```

3. Run the application:

   ```bash
   ./build/FractalQT
   ```

---

## **Usage**

### **Exploring Fractals**

1. **Zoom and Pan**:

   - Use the mouse wheel to zoom in/out.
   - Drag with the right mouse button to pan across the fractal.

2. **Switch Fractal Types**:

   - Select between Mandelbrot and Julia sets using the UI controls.

3. **Adjust Julia Parameters**:

   - Move the mouse pointer in the Mandelbrot view to update the Julia set dynamically.

### **Customizing Palettes**

1. **Predefined Palettes**:
   - Select from a dropdown menu to switch between palettes.
2. **Load Custom Palettes**:
   - Place `.map` files in the `colormap` directory to load them automatically.

### **Performance Settings**

1. **Adjust Iteration Count**:
   - Use the slider to set the maximum iteration count.
2. **Smooth Coloring**:
   - Enable gradient-based smooth coloring for smoother visuals.

---

## **Code Overview**

### **Key Classes**

1. **`MainWindow`**:

   - Manages the application’s main UI and interactions.
   - Handles palette selection and event-based updates.

2. **`MandelBrotView`**:

   - Handles rendering and user interaction for Mandelbrot and Julia views.
   - Supports mouse-based zoom, pan, and Julia parameter adjustments.

3. **`FractalRenderer`**:

   - Core class for fractal computation.
   - Implements multithreaded rendering and dynamic palette updates.

4. **`FractalWorker`**:

   - Worker class for parallelized fractal computation.
   - Optimized with OpenMP for multi-core performance.

5. **`ColorLut`**:

   - Manages color lookup tables for palette generation and application.

### **Rendering Pipeline**

1. **Fractal Computation**:

   - Uses escape-time algorithms for Mandelbrot and Julia sets.
   - Supports smooth coloring for visual gradients.

2. **Multithreading**:

   - `QThreadPool` dynamically distributes rendering tasks.
   - Tile-based rendering improves load balancing.

3. **Image Updates**:

   - Uses `QImage` to update and display fractal data efficiently.

---

## **Customization**

### **Adding Custom Palettes**

1. Create a `.map` file with RGB color definitions.
2. Place the file in the `colormap` directory.
3. Restart the application to load the new palette.

### **Modifying Fractal Parameters**

1. Update `INIT_LEFT`, `INIT_TOP`, `INIT_RIGHT`, and `INIT_BOTTOM` in `FractalRenderer` for default zoom levels.
2. Modify `MAX_ITERATION` to increase rendering precision.

### **Enabling GPU Acceleration**

- **CUDA** or **OpenCL** support can be added for GPU-based fractal computation (not included by default).

---

## **Contributing**

1. Fork the repository.
2. Create a feature branch:
   ```bash
   git checkout -b feature/my-new-feature
   ```
3. Commit your changes:
   ```bash
   git commit -m 'Add some feature'
   ```
4. Push to the branch:
   ```bash
   git push origin feature/my-new-feature
   ```
5. Open a pull request.

---

## **License**

This project is licensed under the MIT License. See the `LICENSE` file for details.

---

## **Acknowledgments**

- Special thanks to contributors and the Qt community for their support.
- Inspired by the beauty and complexity of fractal geometry.

## Future Work

### Optimizing(Speed up)

1. Use Adaptive Sampling

- Problem:

   Currently, your renderer calculates every pixel independently, even for regions that are inside the Mandelbrot or Julia set and will have the same color.

- Solution:

   Use adaptive sampling to skip redundant calculations:

- Divide the image into blocks.

   Check if all pixels in a block converge to the same value (e.g., by evaluating the corners of the block).
   If yes, assign the same value to the entire block. Otherwise, subdivide the block further.

- Implementation:

   ```
   void adaptiveSampling(int x0, int y0, int x1, int y1, int depth) {
      // Evaluate the corners of the block
      int value1 = calcPoint(convertToComplex(x0, y0));
      int value2 = calcPoint(convertToComplex(x1, y0));
      int value3 = calcPoint(convertToComplex(x0, y1));
      int value4 = calcPoint(convertToComplex(x1, y1));

      // If all corners have the same value, fill the block
      if (value1 == value2 && value2 == value3 && value3 == value4) {
         fillBlock(x0, y0, x1, y1, value1);
      } else if (depth < MAX_DEPTH) {
         // Subdivide the block
         int midX = (x0 + x1) / 2;
         int midY = (y0 + y1) / 2;
         adaptiveSampling(x0, y0, midX, midY, depth + 1);
         adaptiveSampling(midX, y0, x1, midY, depth + 1);
         adaptiveSampling(x0, midY, midX, y1, depth + 1);
         adaptiveSampling(midX, midY, x1, y1, depth + 1);
      } else {
         // At maximum depth, calculate each pixel
         for (int y = y0; y < y1; ++y) {
               for (int x = x0; x < x1; ++x) {
                  image[y * width + x] = calcPoint(convertToComplex(x, y));
               }
         }
      }
   }
   ```

- Benefits:

   Reduces redundant calculations in uniform areas.
   Provides substantial speedups, especially for smooth or repetitive regions.

2. Escape-Time Optimization

- Problem:

   You're calculating the number of iterations (calcPoint) for each pixel independently, which is inefficient for regions with known patterns.

- Solution:

   Periodic Detection: 
   
   Many points in fractals eventually repeat values in a periodic loop. Detect this and stop early.

   ```
   if (std::norm(z) < threshold && z == prevZ) {
      return i;
   }
   ```

   Bailout Distance:

   Instead of checking if |z| > 1000, use a lower bailout distance (e.g., 4.0) since larger values won't change the outcome.

   Smooth Coloring: 
   
   Instead of discrete color steps, calculate smooth gradients:

   ```
   double norm = std::sqrt(zr2 + zi2);
   double smoothColor = i + 1 - std::log2(std::log2(norm));
   ```

- Benefits:

   Reduces unnecessary iterations.
   Improves visual quality by avoiding banding.

3. Use Iteration Limits Dynamically

- Problem:

   Setting a fixed MAX_ITERATION for all regions wastes computation on areas where fewer iterations suffice (e.g., outside the main set).

- Solution:
   
   Assign lower iteration counts for points far from the set boundary.
   Increase the iteration count adaptively for zoomed-in regions.

- Implementation:

   ```
   int calcPointDynamicIterations(Complex c) {
      int maxIter = estimateIterations(c);
      for (int i = 0; i < maxIter; ++i) {
         z = z * z + c;
         if (std::norm(z) > 4.0) return i;
      }
      return maxIter;
   }
   ```

4. Parallelize by Tiles

- Problem:

   Even with multithreading, distributing work by rows may result in load imbalance if certain rows are more computationally expensive.

- Solution:

   Use a tile-based parallelism approach:

   Divide the image into smaller tiles (e.g., 32×32 or 64×64).
   Dynamically assign tiles to threads.

- Implementation:

   ```
   #pragma omp parallel for schedule(dynamic, 1)
   for (int ty = 0; ty < height; ty += TILE_SIZE) {
      for (int tx = 0; tx < width; tx += TILE_SIZE) {
         renderTile(tx, ty, TILE_SIZE, TILE_SIZE);
      }
   }
   ```

- Benefits:

   Balances the workload dynamically.
   Reduces synchronization overhead.

5. Use Histogram Equalization for Colors

- Problem:

   Mapping iterations directly to colors can cause uneven color distribution, especially for high iteration counts.

- Solution:

   Build a histogram of iteration counts.
   Adjust the color mapping to distribute colors evenly across the range.

- Implementation:

   ```
   std::vector<int> histogram(MAX_ITERATION, 0);
   for (int i = 0; i < width * height; ++i) {
      ++histogram[iterationCounts[i]];
   }

   // Normalize colors
   int totalPixels = width * height;
   std::vector<int> colorMap(MAX_ITERATION);
   int cumulative = 0;
   for (int i = 0; i < MAX_ITERATION; ++i) {
      cumulative += histogram[i];
      colorMap[i] = (255 * cumulative) / totalPixels;
   }
   ```

6. Precompute Parameters

- Problem:

   Each pixel recalculates the same scaling and translation values repeatedly.

- Solution:
   
   Precompute values like:

   xCoords = left + x * xInterval for all columns.

   yCoords = top + y * yInterval for all rows.

- Implementation:

   ```
   std::vector<double> xCoords(width);
   std::vector<double> yCoords(height);
   for (int x = 0; x < width; ++x) {
      xCoords[x] = left + x * xInterval;
   }
   for (int y = 0; y < height; ++y) {
      yCoords[y] = top + y * yInterval;
   }
   Use these arrays in the main loop:

   for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
         Complex point(xCoords[x], yCoords[y]);
         // Calculate fractal value
      }
   }
   ```

- Benefits:

   Removes repetitive calculations.
   Improves cache locality.

7. Optimize Rendering Pipeline

- Problem:

   Rendering may be limited by memory or I/O bandwidth.

- Solution:

   Render smaller regions into buffers and combine them.
   Use QImage::bits() efficiently to update only the changed regions.

8. Apply Quadtree Data Structures

- Problem:

   Uniform grids waste time calculating pixels in uniform regions.

- Solution:
   
   Use a quadtree:

   Divide the image into quadrants.

   Stop subdividing if the region has a uniform color or reaches a size limit.

   Final Optimized Loop Example

   Combining these techniques:

   ```
   #pragma omp parallel for schedule(dynamic, 1)
   for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
         Complex c = convertToComplex(x, y);
         int iterations = calcPointDynamicIterations(c);

         // Use adaptive sampling
         if (isUniformBlock(x, y)) {
               fillBlock(x, y, iterations);
         } else {
               image[y * width + x] = iterations;
         }
      }
   }
   ```

Expected Speedups

   Adaptive Sampling: Reduces work by skipping redundant calculations.
   Escape-Time Optimization: Shortens unnecessary iteration loops.
   Dynamic Iterations: Tailors precision to computational needs.
   Tile-Based Parallelism: Ensures even distribution of workload across threads.
   Histogram Equalization: Enhances visual quality without adding computational cost.

### How to use CUDA or OpenCL

To use CUDA or OpenCL for accelerating your fractal generation, you’ll need to offload computation-heavy tasks, such as iterating through the Mandelbrot or Julia set formulas, to the GPU. Below is a step-by-step guide for implementing GPU acceleration with both CUDA and OpenCL.

#### CUDA Implementation

CUDA is a parallel computing platform from NVIDIA for running computations on NVIDIA GPUs. Here’s how to implement CUDA in your fractal generator.

---

Step 1: Install CUDA Toolkit

Install the NVIDIA CUDA Toolkit from NVIDIA's website.
Ensure you have an NVIDIA GPU and appropriate drivers installed.
Configure your development environment (e.g., Visual Studio or GCC).

---

Step 2: Write the CUDA Kernel

The CUDA kernel runs on the GPU, and each thread computes a pixel of the fractal.

Example CUDA Kernel:

```
__global__ void renderFractalKernel(int width, int height, double left, double top, double xInterval, double yInterval, int maxIterations, int* output, bool isJulia, double cr, double ci) {
    int x = blockIdx.x * blockDim.x + threadIdx.x; // Compute pixel X coordinate
    int y = blockIdx.y * blockDim.y + threadIdx.y; // Compute pixel Y coordinate

    if (x >= width || y >= height) return;

    double zr = left + x * xInterval;
    double zi = top + y * yInterval;

    double cr_local = cr, ci_local = ci;
    if (!isJulia) {
        cr_local = zr;
        ci_local = zi;
        zr = 0;
        zi = 0;
    }

    int iteration = 0;
    while (zr * zr + zi * zi <= 4.0 && iteration < maxIterations) {
        double zr2 = zr * zr - zi * zi + cr_local;
        zi = 2.0 * zr * zi + ci_local;
        zr = zr2;
        iteration++;
    }

    output[y * width + x] = (255 * iteration) / maxIterations;
}
```

---

Step 3: Launch the CUDA Kernel

The host code initializes GPU memory and launches the kernel.

Host Code Example:

```
#include <cuda_runtime.h>
#include <iostream>

void renderFractalCUDA(int width, int height, double left, double top, double xInterval, double yInterval, int maxIterations, int* output, bool isJulia, double cr, double ci) {
    int* d_output;
    size_t imageSize = width * height * sizeof(int);

    // Allocate GPU memory
    cudaMalloc(&d_output, imageSize);

    // Define grid and block sizes
    dim3 blockSize(16, 16); // Threads per block
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x,
                  (height + blockSize.y - 1) / blockSize.y);

    // Launch kernel
    renderFractalKernel<<<gridSize, blockSize>>>(width, height, left, top, xInterval, yInterval,
                                                 maxIterations, d_output, isJulia, cr, ci);

    // Copy results back to host
    cudaMemcpy(output, d_output, imageSize, cudaMemcpyDeviceToHost);

    // Free GPU memory
    cudaFree(d_output);
}
```

---

Step 4: Integrate CUDA with Your Application

Call renderFractalCUDA from your main application.
Use the output array to update your image buffer.

Main Function Example:

```
int* output = new int[width * height];
renderFractalCUDA(width, height, left, top, xInterval, yInterval, maxIterations, output, isJulia, cr, ci);
// Use `output` to update your image
delete[] output;

```

---

Advantages of CUDA

High Performance: Optimized for NVIDIA GPUs.
Easy Integration: Straightforward kernel launching and memory management.
Rich Ecosystem: Tools like Nsight for debugging and profiling.

### OpenCL Implementation

OpenCL is a cross-platform parallel computing framework that works with GPUs from NVIDIA, AMD, Intel, and others.

---

Step 1: Setup OpenCL

Install the OpenCL SDK (e.g., Intel OpenCL SDK or NVIDIA OpenCL Toolkit).
Configure your build environment to include OpenCL headers and libraries.

---

Step 2: Write the OpenCL Kernel

The OpenCL kernel is similar to the CUDA kernel but uses a slightly different syntax.

Example OpenCL Kernel:

```
__kernel void renderFractal(int width, int height, double left, double top, double xInterval, double yInterval, int maxIterations, __global int* output, int isJulia, double cr, double ci) {
    int x = get_global_id(0); // Pixel X coordinate
    int y = get_global_id(1); // Pixel Y coordinate

    if (x >= width || y >= height) return;

    double zr = left + x * xInterval;
    double zi = top + y * yInterval;

    double cr_local = cr, ci_local = ci;
    if (!isJulia) {
        cr_local = zr;
        ci_local = zi;
        zr = 0;
        zi = 0;
    }

    int iteration = 0;
    while (zr * zr + zi * zi <= 4.0 && iteration < maxIterations) {
        double zr2 = zr * zr - zi * zi + cr_local;
        zi = 2.0 * zr * zi + ci_local;
        zr = zr2;
        iteration++;
    }

    output[y * width + x] = (255 * iteration) / maxIterations;
}
```

---

Step 3: Launch the OpenCL Kernel

Compile the kernel at runtime or pre-build it.
Use OpenCL commands to set up memory and launch the kernel.

Host Code Example:

```
cl::Context context = ...;   // Create an OpenCL context
cl::Program program = ...;   // Build the OpenCL program
cl::CommandQueue queue(context, device);

cl::Buffer outputBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * width * height);

// Set kernel arguments
cl::Kernel kernel(program, "renderFractal");
kernel.setArg(0, width);
kernel.setArg(1, height);
kernel.setArg(2, left);
kernel.setArg(3, top);
kernel.setArg(4, xInterval);
kernel.setArg(5, yInterval);
kernel.setArg(6, maxIterations);
kernel.setArg(7, outputBuffer);
kernel.setArg(8, isJulia);
kernel.setArg(9, cr);
kernel.setArg(10, ci);

// Define global and local work sizes
cl::NDRange globalSize(width, height);
cl::NDRange localSize(16, 16);

// Launch the kernel
queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, localSize);
queue.finish();

// Read back the results
queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(int) * width * height, output);
```

Advantages of OpenCL

Cross-Platform: Works on NVIDIA, AMD, and Intel GPUs, as well as CPUs and FPGAs.
Fine-Grained Control: Explicit memory management and task synchronization.
Portability: Write once, run anywhere.

### CUDA vs. OpenCL

| Feature | #CUDA | #OpenCL |
--- | --- | --- |
| Hardware | NVIDIA GPUs only | Cross-vendor support |
| Ease of Use | Easier (NVIDIA-specific APIs) | Steeper learning curve | 
| Performance | Highly optimized for NVIDIA | Dependent on hardware vendor |
| Portability | Limited to NVIDIA GPUs | Runs on multiple platforms |

## Coloring

1. Smooth Iteration Coloring

   Method:

   Traditional coloring uses the raw iteration count n. 
   Smooth iteration coloring interpolates fractional values for smoother gradients.

   Formula:
   
   $ColorValue=n+1−log⁡2(log⁡2(∣Z∣))$

   Result:

   Produces continuous gradients without abrupt color bands.
   Gives the fractal a more natural and aesthetically pleasing look.

   Implementation:

   ```
   QColor smoothColor(int iterations, double magnitude, int maxIterations) {
      if (iterations == maxIterations) return QColor(Qt::black); // Inside the set
      double mu = iterations + 1 - log(log(magnitude)) / log(2.0);
      double t = mu / maxIterations; // Normalize to [0, 1]
      return QColor::fromHsvF(t, 1.0, 1.0); // Smooth gradient
   }
   ```
   <img src="screenshots/Smooth Coloring Mandelbrot Set.png">


2. Escape-Time Coloring

   Method:

   Map iteration counts to discrete color bands.
   Use modulo operation to cycle through a color palette.
   Colors repeat periodically.

   Result:

   Classic “banded” fractal images with distinct layers.

   Implementation:

   ```
   QColor escapeTimeColor(int iterations, int maxIterations, const std::vector<QColor>& palette) {
      if (iterations == maxIterations) return QColor(Qt::black); // Inside the set
      return palette[iterations % palette.size()];
   }
   ```

3. Orbit Trap Coloring

   Method:

   Tracks the orbit (path of a point under iteration) and colors based on proximity to predefined shapes (e.g., a circle or line).
   Assign colors when the orbit comes close to the shape.

   Result:

   Creates unique and varied visual patterns.

   Implementation:

   ```
   QColor orbitTrapColor(const Complex& z, int iterations, int maxIterations) {
      double distance = std::abs(z - Complex(0.5, 0.5)); // Distance to the trap
      double normalized = std::min(distance, 1.0); // Clamp to [0, 1]
      return QColor::fromRgbF(1.0 - normalized, normalized, 0.5);
   }
   ```
   <img src="screenshots/Orbit Trap Coloring Mandelbrot Set.png">

4. Distance Estimation Coloring

   Method:

   Colors based on the estimated distance of each point from the fractal boundary.
   The closer a point is to the boundary, the darker it is.

   Result:

   Smooth and detailed boundaries with a gradient effect.

   Implementation:

   ```
   QColor distanceEstimationColor(const Complex& z, int iterations, int maxIterations) {
      double distance = std::abs(z); // Calculate distance
      double normalized = iterations / double(maxIterations);
      return QColor::fromRgbF(normalized, 0.5, 1.0 - normalized);
   }
   ```

5. Custom Gradient Palettes

   Method:

   Use pre-designed gradient palettes for unique looks.
   Map iteration counts to colors in the gradient.

   Result:

   Enables custom, artistic visuals.

   Implementation:

   ```
   QColor gradientColor(double value, const std::vector<QColor>& gradient) {
      int index = value * (gradient.size() - 1); // Map to gradient index
      return gradient[index];
   }
   ```

   <img src="screenshots/Palette-Based Blending Mandelbrot Set.png">

### Sample Coloring Results

1. Smooth Iteration:

   Continuous gradients with no visible bands.

2. Escape-Time:

   Vibrant, periodic bands highlighting iteration regions.

3. Orbit Traps:

   Patterns determined by proximity to a specific shape.

4. Distance Estimation:

   Smoothly darkens points as they approach the set boundary.

5. Custom Gradient Palettes:

   Tailored color designs based on artistic choices.


## References

<a href = 'https://en.wikipedia.org/wiki/Mandelbrot_set#:~:text=The%20Mandelbrot%20set%20(%2F%CB%88m,remains%20bounded%20in%20absolute%20value.'>Mandelbrot set</a>

<a href = "https://mateuszsiedlecki.com/index.php/2020/05/27/drawing-fractals-in-qt-using-threads/">Drawing fractals in Qt using threads</a>

<a href = 'https://lodev.org/cgtutor/juliamandelbrot.html'>Lode's Computer Graphics Tutorial</a>

<a href = 'https://scipython.com/book/chapter-7-matplotlib/problems/p72/the-julia-set/'>The Julia set using python</a>

<a href='http://www.alunw.freeuk.com/mandelbrotroom.html'>The Mandelbrot set and Julia sets</a>

<a href='https://www.karlsims.com/julia.html'>
Understanding Julia and Mandelbrot Sets, Karl Sims</a>

<a href='http://bl.ocks.org/syntagmatic/3736720'>Zoomable Julia Set</a>
