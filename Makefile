default: release

.PHONY: default release debug all clean

include make-utils/flags.mk
include make-utils/cpp-utils.mk

CXX_FLAGS += -Idll/include -Idll/etl/include -Idll/etl/lib/include -Imnist/include -std=c++1y -stdlib=libc++
LD_FLAGS += -lopencv_core -lopencv_imgproc -lopencv_highgui -ljpeg -lpthread

$(eval $(call auto_folder_compile,src))
$(eval $(call add_src_executable,rbm_mnist,rbm_mnist.cpp))

release: release/bin/rbm_mnist
debug: debug/bin/rbm_mnist

all: release debug

clean: base_clean

include make-utils/cpp-utils-finalize.mk
