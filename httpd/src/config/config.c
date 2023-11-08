#include "config.h"

/*
** @brief Parse the configuration file and return a config struct
**        The config struct must be freed with config_destroy
**        If an error occurs, the function returns NULL
**
** @param path The path to the configuration file
**
** @return The config struct or NULL if an error occurs
*/
struct config *parse_configuration(const char *path)
{
//TODO
}

/*
** @brief Free the config struct
**
** @param config The config struct to free
*/
void config_destroy(struct config *config)
{
//TODO
}
