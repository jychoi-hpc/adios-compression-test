# Adios compression test

This is a code for testing Adios compression methods. It will read one variable of a given Adios file and write the variable into another Adios file. During the process, we can specify Adios compression options through the xml config file. 

# Building
```
mkdir build
cd build
CXX=g++ cmake .. 
make
```


# Run
The usage is as follows:
```
./adios-rewrite filename varname output
```

Here is an example:
```
./adios-rewrite xgc.3d.00304.bp dpot out.bp
```

In the above command, it will read `dpot` variable in `xgc.3d.00304.bp` and write to `out.bp`. It requires an adios xml file (`adios2.cfg`). 
To test with compression, comment out compression section in `adios2.xml`. An example is as follows:

```
<?xml version="1.0"?>
<adios-config>

    <!--============================================
           Reader
        ============================================-->

    <io name="Reader">
        <engine type="BP4">
        </engine>
    </io>

    <!--===========================================
           Writer
        ===========================================-->

    <io name="Writer">
        <engine type="BP4">
        </engine>

        <!-- Compress variables -->
        <!-- variable name "out" is fixed -->
        <variable name="out">
            <operation type="mgard">
                <parameter key="accuracy" value="0.01"/>
                <parameter key="gpu" value="1"/>
            </operation>
        </variable>
    </io>

</adios-config>
```