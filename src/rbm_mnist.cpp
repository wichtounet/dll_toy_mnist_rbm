//=======================================================================
// Copyright (c) 2015 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <iostream>

#include "dll/rbm.hpp"
#include "dll/ocv_visualizer.hpp"

#include "mnist/mnist_reader.hpp"
#include "mnist/mnist_utils.hpp"

int main(int argc, char* argv[]){
    auto reconstruction = false;
    auto load = false;

    for(int i = 1; i < argc; ++i){
        std::string command(argv[i]);

        if(command == "sample"){
            reconstruction = true;
        } else if(command == "load"){
            load = true;
        }
    }

    auto dataset = mnist::read_dataset<std::vector, std::vector, double>(5000);

    if(dataset.training_images.empty() || dataset.training_labels.empty()){
        std::cout << "Impossible to read dataset" << std::endl;
        return 1;
    }

    mnist::binarize_dataset(dataset);

    using rbm_t = dll::rbm_desc<
           28 * 28,
           200,
           dll::parallel,
           dll::momentum,
           dll::weight_decay<dll::decay_type::L2>,
           dll::sparsity<dll::sparsity_method::LOCAL_TARGET>,
           //dll::trainer_rbm<dll::pcd1_trainer_t>,
            dll::batch_size<24>,
           //dll::hidden<dll::unit_type::RELU>,
           dll::watcher<dll::opencv_rbm_visualizer>
       >::rbm_t;

    rbm_t rbm;

    //rbm.momentum = 0.9;
    rbm.sparsity_target = 0.01;
    //rbm.sparsity_cost = 0.9;
    //rbm.learning_rate /= 10.0;

    if(load){
        std::ifstream is("rbm-1.dat", std::ofstream::binary);
        rbm.load(is);
    } else {
        rbm.train(dataset.training_images, 25);

        std::ofstream os("rbm-1.dat", std::ofstream::binary);
        rbm.store(os);
    }

    if(reconstruction){
        for(size_t t = 0; t < 10; ++t){
            auto& image = dataset.training_images[6 + t];

            std::cout << "Source image" << std::endl;
            for(size_t i = 0; i < 28; ++i){
                for(size_t j = 0; j < 28; ++j){
                    std::cout << static_cast<size_t>(image[i * 28 + j]) << " ";
                }
                std::cout << std::endl;
            }

            rbm.reconstruct(image);

            std::cout << "Reconstructed image" << std::endl;
            std::cout.precision(2);
            rbm.display_visible_units(28);
        }
    }

    return 0;
}
