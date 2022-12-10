#ifndef REGION_H
# define REGION_H

# include <stdint.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>

# define MAX_REGION 30

typedef struct s_ft_president
{
	char		*names[MAX_REGION];
	uint8_t		neighbors[MAX_REGION][MAX_REGION];
	int32_t		pibs[MAX_REGION];
	uint32_t	mean;
	uint8_t		count;
}	t_ft_president;

uint8_t	init(t_ft_president *president, const char *count);

char	*read_data();
uint8_t	parse(char *data, t_ft_president *president);

#endif // REGION_H
