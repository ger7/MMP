/*
  Copyright (C) 2003-2013 Paul Brossier <piem@aubio.org>

  This file is part of aubio.

  aubio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  aubio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with aubio.  If not, see <http://www.gnu.org/licenses/>.

*/

extern int verbose;
// input / output
extern int usejack;
extern char_t *source_uri;
extern char_t *sink_uri;
// general stuff
extern uint_t samplerate;
extern uint_t buffer_size;
extern uint_t hop_size;
// onset stuff
extern char_t * onset_method;
extern smpl_t onset_threshold;
// pitch stuff
extern char_t * pitch_method;
extern char_t * pitch_unit;
extern smpl_t pitch_tolerance;
// tempo stuff
extern char_t * tempo_method;
// more general stuff
extern smpl_t silence_threshold;
extern uint_t mix_input;

extern uint_t force_overwrite;



// internal stuff
extern int blocks;

extern fvec_t *ibuf;
extern fvec_t *obuf;

const char *prog_name;

void
usage (FILE * stream, int exit_code)
{
  fprintf (stream, "usage: %s [ options ] \n", prog_name);
  fprintf (stream,
      "       -i      --input            input file\n"
#ifdef PROG_HAS_OUTPUT
      "       -o      --output           output file\n"
#endif
      "       -r      --samplerate       select samplerate\n"
      "       -B      --bufsize          set buffer size\n"
      "       -H      --hopsize          set hopsize\n"
#ifdef PROG_HAS_ONSET
      "       -O      --onset            select onset detection algorithm\n"
      "       -t      --onset-threshold  set onset detection threshold\n"
#endif /* PROG_HAS_ONSET */
#ifdef PROG_HAS_PITCH
      "       -p      --pitch            select pitch detection algorithm\n"
      "       -u      --pitch-unit       select pitch output unit\n"
      "       -l      --pitch-tolerance  select pitch tolerance\n"
#endif /* PROG_HAS_PITCH */
      "       -s      --silence          select silence threshold\n"
#ifdef PROG_HAS_OUTPUT
      "       -m      --mix-input        mix input signal with output signal\n"
      "       -f      --force-overwrite  overwrite output file if needed\n"
#endif
#ifdef PROG_HAS_JACK
      "       -j      --jack             use Jack\n"
#endif
      "       -v      --verbose          be verbose\n"
      "       -h      --help             display this message\n"
      );
  exit (exit_code);
}

int
parse_args (int argc, char **argv)
{
  const char *options = "hv"
    "i:r:B:H:"
#ifdef PROG_HAS_JACK
    "j"
#endif /* PROG_HAS_JACK */
#ifdef PROG_HAS_OUTPUT
    "o:"
#endif /* PROG_HAS_OUTPUT */
#ifdef PROG_HAS_ONSET
    "O:t:"
#endif /* PROG_HAS_ONSET */
#ifdef PROG_HAS_PITCH
    "p:u:l:"
#endif /* PROG_HAS_PITCH */
    "s:mf";
  int next_option;
  struct option long_options[] = {
    {"help",                  0, NULL, 'h'},
    {"verbose",               0, NULL, 'v'},
    {"input",                 1, NULL, 'i'},
    {"samplerate",            1, NULL, 'r'},
    {"bufsize",               1, NULL, 'B'},
    {"hopsize",               1, NULL, 'H'},
#ifdef PROG_HAS_JACK
    {"jack",                  0, NULL, 'j'},
#endif /* PROG_HAS_JACK */
#ifdef PROG_HAS_OUTPUT
    {"output",                1, NULL, 'o'},
#endif /* PROG_HAS_OUTPUT */
#ifdef PROG_HAS_ONSET
    {"onset",                 1, NULL, 'O'},
    {"onset-threshold",       1, NULL, 't'},
#endif /* PROG_HAS_ONSET */
#ifdef PROG_HAS_PITCH
    {"pitch",                 1, NULL, 'p'},
    {"pitch-unit",            1, NULL, 'u'},
    {"pitch-tolerance",       1, NULL, 'l'},
#endif /* PROG_HAS_PITCH */
    {"silence",               1, NULL, 's'},
    {"mix-input",             0, NULL, 'm'},
    {"force-overwrite",       0, NULL, 'f'},
    {NULL,                    0, NULL, 0}
  };
  prog_name = argv[0];
  if (argc < 1) {
    usage (stderr, 1);
    return -1;
  }
  do {
    next_option = getopt_long (argc, argv, options, long_options, NULL);
    switch (next_option) {
      case 'h':                /* help */
        usage (stdout, 0);
        return -1;
      case 'v':                /* verbose */
        verbose = 1;
        break;
      case 'j':
        usejack = 1;
        break;
      case 'i':
        source_uri = optarg;
        break;
      case 'o':
        sink_uri = optarg;
        break;
      case 'f':                /* force_overwrite flag */
        force_overwrite = 1;
        break;
      case 'r':
        samplerate = atoi (optarg);
        break;
      case 'B':
        buffer_size = atoi (optarg);
        break;
      case 'H':
        hop_size = atoi (optarg);
        break;
      case 'O':                /*onset method */
        onset_method = optarg;
        break;
      case 't':                /* threshold value for onset */
        onset_threshold = (smpl_t) atof (optarg);
        break;
      case 'p':
        pitch_method = optarg;
        break;
      case 'u':
        pitch_unit = optarg;
        break;
      case 'l':
        pitch_tolerance = (smpl_t) atof (optarg);
        break;
      case 's':                /* silence threshold */
        silence_threshold = (smpl_t) atof (optarg);
        break;
      case 'm':                /* mix_input flag */
        mix_input = 1;
        break;
      case '?':                /* unknown options */
        usage (stderr, 1);
        break;
      case -1:                 /* done with options */
        break;
      default:                 /*something else unexpected */
        fprintf (stderr, "Error parsing option '%c'\n", next_option);
        abort ();
    }
  }
  while (next_option != -1);

  // if unique, use the non option argument as the source
  if ( source_uri == NULL ) {
    if (argc - optind == 1) {
      source_uri = argv[optind];
    } else if ( argc - optind > 1 ) {
      errmsg ("Error: too many non-option arguments `%s'\n", argv[argc - 1]);
      usage ( stderr, 1 );
    }
  } else if ( argc - optind > 0 ) {
    errmsg ("Error: extra non-option argument %s\n", argv[optind]);
    usage ( stderr, 1 );
  }

  // if no source, show a message
  if (source_uri == NULL) {
#ifdef PROG_HAS_JACK
#if HAVE_JACK
    verbmsg("No input source given, using jack\n");
    usejack = 1;
#else
    errmsg("Error: no arguments given (and no available audio input)\n");
    usage ( stderr, 1 );
#endif /* HAVE_JACK */
#else
    errmsg("Error: no arguments given\n");
    usage ( stderr, 1 );
#endif /* PROG_HAS_JACK */
  }

  if ((sint_t)hop_size < 1) {
    errmsg("Error: got hop_size %d, but can not be < 1\n", hop_size);
    usage ( stderr, 1 );
  } else if ((sint_t)buffer_size < 2) {
    errmsg("Error: got buffer_size %d, but can not be < 2\n", buffer_size);
    usage ( stderr, 1 );
  } else if ((sint_t)buffer_size < (sint_t)hop_size + 1) {
    errmsg("Error: hop size (%d) is larger than or equal to win size (%d)\n",
        hop_size, buffer_size);
    usage ( stderr, 1 );
  }

  if ((sint_t)samplerate < 0) {
    errmsg("Error: got samplerate %d, but can not be < 0\n", samplerate);
    usage ( stderr, 1 );
  }

  return 0;
}
