#include "test_utils.hpp"

#include <ds/frame_of_discernment.hpp>

int main()
{
    using Focal4 = ds::FocalSet<4>;

    const Focal4 empty;
    DS_CHECK(ds::is_empty(empty));
    DS_CHECK(empty.count() == 0);

    const Focal4 singleton{0b0001};
    DS_CHECK(!ds::is_empty(singleton));
    DS_CHECK(singleton.count() == 1);

    const Focal4 a{0b0011};
    const Focal4 b{0b0110};
    DS_CHECK((a | b) == Focal4{0b0111});
    DS_CHECK((a & b) == Focal4{0b0010});
    DS_CHECK(ds::is_empty(Focal4{0b0001} & Focal4{0b0010}));

    DS_CHECK(ds::is_empty(ds::FocalSet<1>{}));
    DS_CHECK(!ds::is_empty(ds::FocalSet<8>{0b1000'0000}));

    return ds_test::summary_and_exit("frame_of_discernment");
}
