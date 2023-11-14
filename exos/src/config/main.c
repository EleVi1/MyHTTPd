#include "config.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    struct config *conf = parse_configuration("../test3");
    config_destroy(conf);
    return 0;
}
