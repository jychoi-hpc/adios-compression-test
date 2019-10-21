/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * helloreader.cpp: Simple self-descriptive example of how to read a variable
 * to a BP File.
 *
 * Try running like this from the build directory:
 *   mpirun -np 3 ./bin/hello_reader
 *
 *  Created on: Feb 16, 2017
 *      Author: William F Godoy godoywf@ornl.gov
 */
#include <ios>      //std::ios_base::failure
#include <iostream> //std::cout
#include <mpi.h>
#include <stdexcept> //std::invalid_argument std::exception
#include <vector>

#include <adios2.h>

int main(int argc, char *argv[])
{
    // Check arguments
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " filename variable outfile" << std::endl;
        return 1;
    }

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    std::string configfile = "adios2.xml";
    std::string filename = argv[1];
    std::string varname = argv[2];
    std::string outfile = argv[3];

    try
    {
        std::vector<std::size_t> shape;
        std::vector<std::size_t> offset;
        std::vector<std::size_t> count;
        std::vector<double> val;
        std::vector<double> val2;
        double t1, t2;

        adios2::ADIOS adios(configfile, MPI_COMM_WORLD, adios2::DebugON);

        // Reading
        adios2::IO reader_io = adios.DeclareIO("Reader");
        t1 = MPI_Wtime();
        adios2::Engine reader = reader_io.Open(filename, adios2::Mode::Read);
        adios2::Variable<double> var_in = reader_io.InquireVariable<double>(varname);
        shape = var_in.Shape();
        offset = shape;
        count = shape;
        // we assume using only a single process
        for (int i=0; i<shape.size(); i++) 
        {
            offset[i] = 0;
            count[i] = shape[i];
        }

        reader.Get<double>(var_in, val);
        reader.Close();
        t2 = MPI_Wtime();
        std::cout << ">>> Raw-data loading time (sec): " << t2-t1 << std::endl;

        // Writing (for compression)
        adios2::IO writer_io = adios.DeclareIO("Writer");
        t1 = MPI_Wtime();
        adios2::Engine writer = writer_io.Open(outfile, adios2::Mode::Write, MPI_COMM_WORLD);
        adios2::Variable<double> var2 = writer_io.DefineVariable<double>("out", shape, offset, count);
        writer.Put<double>(var2, val.data());
        writer.Close();
        t2 = MPI_Wtime();
        std::cout << ">>> Writing time (sec): " << t2-t1 << std::endl;

        // Reading back (for decompression)
        adios2::IO reader2_io = adios.DeclareIO("Reader2");
        t1 = MPI_Wtime();
        adios2::Engine reader2 = reader2_io.Open(outfile, adios2::Mode::Read);
        adios2::Variable<double> var_in2 = reader2_io.InquireVariable<double>("out");
        reader2.Get<double>(var_in2, val2);
        reader2.Close();
        t2 = MPI_Wtime();
        std::cout << ">>> Re-reading time (sec): " << t2-t1 << std::endl;

        // next work: we can calculate errors here
    }
    catch (std::invalid_argument &e)
    {
        if (rank == 0)
        {
            std::cerr
                << "Invalid argument exception, STOPPING PROGRAM from rank "
                << rank << "\n";
            std::cerr << e.what() << "\n";
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    catch (std::ios_base::failure &e)
    {
        if (rank == 0)
        {
            std::cerr << "IO System base failure exception, STOPPING PROGRAM "
                         "from rank "
                      << rank << "\n";
            std::cerr << e.what() << "\n";
            std::cerr << "The file " << filename << " does not exist."
                      << " Presumably this is because hello_bpWriter has not "
                         "been run."
                      << " Run ./hello_bpWriter before running this program.\n";
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    catch (std::exception &e)
    {
        if (rank == 0)
        {
            std::cerr << "Exception, STOPPING PROGRAM from rank " << rank
                      << "\n";
            std::cerr << e.what() << "\n";
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Finalize();

    return 0;
}
