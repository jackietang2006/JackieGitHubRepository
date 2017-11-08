#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct thread_info {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       thread_num;       /* Application-defined thread # */
    int       recursions;       /* Application-defined thread # */
    char     *argv_string;      /* From command-line argument */
};

/* Thread start function: display address near top of our stack,
 *    and return upper-cased copy of argv_string */

void
goinside(int arg)
{
    printf("[%x]: Inside: recursions = %d\n", pthread_self(), arg);

    sleep(10);
  
    if (arg == 0) {
    }
    else {
      char arr[1024000];
      goinside (arg-1);
    }
}

static void *
thread_start(void *arg)
{
    struct thread_info *tinfo = (struct thread_info*)arg;
    char *uargv, *p;

    uargv = strdup(tinfo->argv_string);
    if (uargv == NULL)
        handle_error("strdup");

    printf("[%x-%d]: top of stack near %p; recursions = %d argv_string=%s\n",
            pthread_self(), tinfo->thread_num, uargv, tinfo->recursions, tinfo->argv_string);

    for (p = uargv; *p != '\0'; p++)
        *p = toupper(*p);

    goinside(tinfo->recursions);

    return uargv;
}

int
main(int argc, char *argv[])
{
    int s, tnum, opt, num_threads;
    struct thread_info *tinfo;
    pthread_attr_t attr;
    size_t stacksize;
    int stack_size;
    void *res;

    /* The "-s" option specifies a stack size for our threads */

    stack_size = -1;
    while ((opt = getopt(argc, argv, "s:")) != -1) {
        printf("[%x]: A = %c-%d a = %c-%d\n", pthread_self(), 'A', 'A', 'a', 'a');
        switch (opt) {
        case 's':
            stack_size = strtoul(optarg, NULL, 0);
            break;

        default:
            fprintf(stderr, "Usage: %s [-s stack-size] arg...\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    printf("[%x]: argc=%d optind=%d preferred stack size=%d\n",
            pthread_self(), argc, optind, stack_size);

    sleep(20);

    num_threads = (argc - optind) / 2;

    /* Initialize thread creation attributes */

    s = pthread_attr_init(&attr);

    pthread_attr_getstacksize (&attr, &stacksize);
    printf("[%x]: Default stack size = %li\n", pthread_self(), stacksize);
    printf("[%x]: num_threads = %d\n", pthread_self(), num_threads);

    if (s != 0)
        handle_error_en(s, "pthread_attr_init");

    if (stack_size > 0) {
        s = pthread_attr_setstacksize(&attr, stack_size);
        if (s != 0)
            handle_error_en(s, "pthread_attr_setstacksize");
    }

    /* Allocate memory for pthread_create() arguments */

    tinfo = (struct thread_info*)calloc(num_threads, sizeof(struct thread_info));
    if (tinfo == NULL)
        handle_error("calloc");

    /* Create one thread for each command-line argument */

    for (tnum = 0; tnum < num_threads; tnum++) {
        tinfo[tnum].thread_num = tnum + 1;
        tinfo[tnum].argv_string = argv[optind + tnum*2];
        tinfo[tnum].recursions = atoi(argv[optind + 1 + tnum*2]);

        /* The pthread_create() call stores the thread ID into
         *                   corresponding element of tinfo[] */
        
        s = pthread_create(&tinfo[tnum].thread_id, &attr,
                       &thread_start, &tinfo[tnum]);
        if (s != 0)
            handle_error_en(s, "pthread_create");
            printf("[%x]: Created thread: %x-%d - %s\n",
                pthread_self(),
                tinfo[tnum].thread_id,
                tinfo[tnum].thread_num,
                tinfo[tnum].argv_string);
    }

    /* Destroy the thread attributes object, since it is no
     * longer needed */

    s = pthread_attr_destroy(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_destroy");

    //Now join with each thread, and display its returned value

    for (tnum = 0; tnum < num_threads; tnum++) {
        s = pthread_join(tinfo[tnum].thread_id, &res);
        if (s != 0)
            handle_error_en(s, "pthread_join");

            printf("[%x]: Joined with thread: %x-%d; returned value was %p-%s\n",
              pthread_self(),
              tinfo[tnum].thread_id,
              tinfo[tnum].thread_num, res, (char *) res);
            sleep(10);
            free(res);      /* Free memory allocated by thread */
    }

    free(tinfo);
    exit(EXIT_SUCCESS);
}
