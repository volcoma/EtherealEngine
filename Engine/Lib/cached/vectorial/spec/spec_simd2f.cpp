
#include "spec_helper.h"

const int epsilon = 1;

#ifdef VECTORIAL_HAVE_SIMD2F

describe(simd2f, "sanity") {
    it("VECTORIAL_SIMD_TYPE should be defined to a string") {
        std::cout << "Simd type: " << VECTORIAL_SIMD_TYPE << std::endl;
    }
}

describe(simd2f, "creating") {
    
    it("should be possible to create with simd2f_create") {
        
        simd2f x = simd2f_create(1, 2);

        should_be_close_to( simd2f_get_x(x), 1, epsilon);
        should_be_close_to( simd2f_get_y(x), 2, epsilon);

        // octave simd2f: [1,2]
        should_be_equal_simd2f(x, simd2f_create(1.000000000000000f, 2.000000000000000f), epsilon );
        
    }

    it("should have simd2f_zero for zero vector") {

        simd2f x = simd2f_zero();

        // octave simd2f: [0,0]
        should_be_equal_simd2f(x, simd2f_create(0.000000000000000f, 0.000000000000000f), epsilon );
    }
    
    
}
#ifdef _MSC_VER
#include <malloc.h>
#else
#include <alloca.h>
#endif

#define unaligned_mem(n) ((float*)((unsigned char*)alloca(sizeof(float)*n+4)+4))

describe(simd2f, "utilities") {

    it("should have simd2f_uload2 for loading two float values from float an unaligned array into simd2f") {
        float *f = unaligned_mem(2);
        f[0] = 1;
        f[1] = 2;
        simd2f x = simd2f_uload2(f);
        // octave simd2f: [1,2]
        should_be_equal_simd2f(x, simd2f_create(1.000000000000000f, 2.000000000000000f), epsilon );
    }

    it("should have simd2f_ustore2 for storing two float values from simd2f to an unaligned array") {
        float *f = unaligned_mem(2);
        f[0] = -1;
        f[1] = -1;
        simd2f a = simd2f_create(1,2);
        simd2f_ustore2(a, f);
        should_be_close_to(f[0], 1, epsilon);
        should_be_close_to(f[1], 2, epsilon);
    }


    it("should have simd2f_splat that expands a single scalar to all elements") {
        simd2f x = simd2f_splat(42);
        // octave simd2f: [42,42]
        should_be_equal_simd2f(x, simd2f_create(42.000000000000000f, 42.000000000000000f), epsilon );
    }

    it("should have simd2f_splat_x,y splatting of an element") {
        simd2f a = simd2f_create(1,2);

        simd2f x;
        
        x = simd2f_splat_x(a);
        // octave simd2f: [1,1]
        should_be_equal_simd2f(x, simd2f_create(1.000000000000000f, 1.000000000000000f), epsilon );

        x = simd2f_splat_y(a);
        // octave simd2f: [2,2]
        should_be_equal_simd2f(x, simd2f_create(2.000000000000000f, 2.000000000000000f), epsilon );

    }

#if 0
    it("should have simd2f_sum that adds elements") {
        simd2f a = simd2f_create(1,2);
        simd2f x = simd2f_sum(a);
        // octave simd2f: [sum([1,2]), sum([1,2,3,4])]
        should_be_equal_simd2f(x, simd2f_create(3.000000000000000f, 10.000000000000000f), epsilon );
        
    }
#endif

    it("should have simd2f_reciprocal") {
        simd2f a = simd2f_create(0.00001f, 2.00001f);
        simd2f x = simd2f_reciprocal(a);
        // octave simd2f: 1 ./ [0.00001, 2.00001]
        should_be_equal_simd2f(x, simd2f_create(99999.999999999985448f, 0.499997500012500f), epsilon );
    }

    it("should have simd2f_sqrt") {
        simd2f a = simd2f_create(0.00001f, 2.00001f);
        simd2f x = simd2f_sqrt(a);
        // octave simd2f:  sqrt([0.00001, 2.00001])
        should_be_equal_simd2f(x, simd2f_create(0.003162277660168f, 1.414217097902582f), epsilon );

        x = simd2f_sqrt( simd2f_create(0.0f, 0.0f) );
        // octave simd2f:  sqrt([0, 0])
        should_be_equal_simd2f(x, simd2f_create(0.000000000000000f, 0.000000000000000f), epsilon );
    }

    it("should have simd2f_rsqrt for reciprocal of square-root") {
        simd2f a = simd2f_create(0.00001f, 2.00001f);
        simd2f x = simd2f_rsqrt(a);
        const int epsilon = 4; // Grant larger error
        // octave simd2f:  1 ./ sqrt([0.00001, 2.00001])
        should_be_equal_simd2f(x, simd2f_create(316.227766016837904f, 0.707105013426224f), epsilon );
    }

}

describe(simd2f, "arithmetic with another simd2f") {

    it("should have simd2f_add for component-wise addition") {
        simd2f a = simd2f_create(1,2);
        simd2f b = simd2f_create(10,20);
        
        simd2f x = simd2f_add(a,b);
        // octave simd2f: [1,2] + [10,20]
        should_be_equal_simd2f(x, simd2f_create(11.000000000000000f, 22.000000000000000f), epsilon );
    }

    it("should have simd2f_sub for component-wise subtraction") {
        simd2f a = simd2f_create(1,2);
        simd2f b = simd2f_create(10,20);
        
        simd2f x = simd2f_sub(b,a);
        // octave simd2f: [10,20] - [1,2] 
        should_be_equal_simd2f(x, simd2f_create(9.000000000000000f, 18.000000000000000f), epsilon );
    }

    it("should have simd2f_mul for component-wise multiply") {
        simd2f a = simd2f_create(1,2);
        simd2f b = simd2f_create(10,20);
        
        simd2f x = simd2f_mul(a,b);
        // octave simd2f: [1,2] .* [10,20]
        should_be_equal_simd2f(x, simd2f_create(10.000000000000000f, 40.000000000000000f), epsilon );
    }

    it("should have simd2f_div for component-wise division") {
        simd2f a = simd2f_create(1,2);
        simd2f b = simd2f_create(10,20);
        
        simd2f x = simd2f_div(b,a);
        // octave simd2f: [10,20] ./ [1,2] 
        should_be_equal_simd2f(x, simd2f_create(10.000000000000000f, 10.000000000000000f), epsilon );
    }

    it("should have simd2f_madd for multiply-add") {
        simd2f a = simd2f_create(1,2);
        simd2f b = simd2f_create(100,100);
        simd2f c = simd2f_create(6,7);

        simd2f x = simd2f_madd(a,b,c);
        // octave simd2f: [1,2] .* [100,100] .+ [6,7]
        should_be_equal_simd2f(x, simd2f_create(106.000000000000000f, 207.000000000000000f), epsilon );

    }

}


describe(simd2f, "vector math") {

    it("should have simd2f_dot2 for two component dot product") {
        simd2f a = simd2f_create(1,2);
        simd2f b = simd2f_create(10,20);
        
        simd2f x = simd2f_dot2(a,b);
        // octave simd2f: [dot([1, 2], [10, 20]),dot([1, 2], [10, 20])]
        should_be_equal_simd2f(x, simd2f_create(50.000000000000000f, 50.000000000000000f), epsilon );
    }

    it("should have simd2f_length2 for two component vector length") {
        simd2f a = simd2f_create(1,2);
        simd2f x = simd2f_length2(a);
        // octave simd2f: [norm([1,2]),norm([1,2])]
        should_be_equal_simd2f(x, simd2f_create(2.236067977499790f, 2.236067977499790f), epsilon );

    }


    it("should have simd2f_length2_squared for two component squared vector length") {
        simd2f a = simd2f_create(1,2);
        simd2f x = simd2f_length2_squared(a);
        // octave simd2f: ([dot([1,2], [1,2]), dot([1,2], [1,2])])
        should_be_equal_simd2f(x, simd2f_create(5.000000000000000f, 5.000000000000000f), epsilon );

    }

    it("should have simd2f_normalize2 for normalizing two component vector to unit length") {
        simd2f a = simd2f_create(1,2);
        simd2f x = simd2f_normalize2(a);
        // octave simd2f: [1,2] / norm([1,2])
        should_be_equal_simd2f(x, simd2f_create(0.447213595499958f, 0.894427190999916f), epsilon );
    }

}


describe(simd2f, "min-max") {

    it("should have simd2f_min for choosing minimum elements") {
        simd2f a = simd2f_create(1.0f,  2.0f);
        simd2f b = simd2f_create(2.0f, -2.0f);

        simd2f x = simd2f_min(a,b);
        should_be_equal_simd2f(x, simd2f_create(1.0f, -2.0f), epsilon);

    }

    it("should have simd2f_max for choosing maximum elements") {
        simd2f a = simd2f_create(1.0f,  2.0f);
        simd2f b = simd2f_create(2.0f, -2.0f);

        simd2f x = simd2f_max(a,b);
        should_be_equal_simd2f(x, simd2f_create(2.0f, 2.0f), epsilon);

    }

}



#endif

