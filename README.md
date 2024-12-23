# capnproto example

## Summary
This is a data serializer/desiralizer. Which means that if we have a lot of data to write, capnproto can quickly convert it to byte stream / data stream. This is not an alternative of fstream. After creating the byte stream, we need to use fstream to write to file. Conversely, we can use fstream to read the byte stream, and capnproto can convert the byte stream to desiralized data.

## Installation

### On local machine

1. Use anaconda to install (with proper env, activate it)
```
conda install conda-forge::capnproto
```

2. Locate the include and lib paths for the conda environemnt.
For me, these are the following:
```
/home/grads/mbr5797/anaconda3/envs/capnp/include
/home/grads/mbr5797/anaconda3/envs/capnp/lib
```

### Setting up vscode
1. Add the following in `.vscode/c_cpp_properties.json`:
```
"includePath": [
    "${workspaceFolder}/**",
    "/home/grads/mbr5797/anaconda3/envs/capnp/include/"
]
```

Make sure that proper g++/gcc path is available there.

### Setting up g++ (building)
1. Need to add the following in g++ build commands:
```
-lcapnp -lkj -I /home/grads/mbr5797/anaconda3/envs/capnp/include -L /home/grads/mbr5797/anaconda3/envs/capnp/lib
```


### Setting up runtime (after building)
1. Add the lib path to LD_LIBRARY_PATH variable:
```
export LD_LIBRARY_PATH=LD_LIBRARY_PATH=/home/grads/mbr5797/anaconda3/envs/capnp/lib:$LD_LIBRARY_PATH
```

Replace the path accordingly.

## Build instruction:
```
capnp compile -oc++ map.capnp
g++ -std=c++17 -O2 -o main example.cpp map.capnp.c++ -lcapnp -lkj -I /home/grads/mbr5797/anaconda3/envs/capnp/include -L /home/grads/mbr5797/anaconda3/envs/capnp/lib
```

## Run
```
./main
```