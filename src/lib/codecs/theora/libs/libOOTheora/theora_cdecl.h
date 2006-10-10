/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggTheora SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE Theora SOURCE CODE IS COPYRIGHT (C) 2002-2003                *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

  function:
  last mod: $Id: theora.h,v 1.18 2004/03/09 06:18:44 msmith Exp $

 ********************************************************************/

#ifndef _O_THEORA_H_
#define _O_THEORA_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef LIBOGG2
#include <ogg/ogg.h>
#else
#include <ogg2/ogg.h>
/* This is temporary until libogg2 is more complete */
ogg_buffer_state *ogg_buffer_create(void);
#endif

/** \mainpage
 * 
 * \section intro Introduction
 *
 * This is the documentation for the libtheora C API.
 * libtheora is the reference implementation for
 * <a href="http://www.theora.org/">Theora</a>, a free video codec.
 * Theora is derived from On2's VP3 codec with improved integration for
 * Ogg multimedia formats by <a href="http://www.xiph.org/">Xiph.Org</a>.
 * 
 * \section overview Overview
 *
 * This library will both decode and encode theora packets to/from raw YUV 
 * frames.  In either case, the packets will most likely either come from or
 * need to be embedded in an ogg stream.  Use 
 * <a href="http://xiph.org/ogg/">libogg</a> or 
 * <a href="http://www.annodex.net/software/liboggz/index.html">liboggz</a>
 * to extract/package these packets.
 *
 * \section decoding Decoding Process
 *
 * Decoding can be seperated into the following steps:
 * -# initialise theora_info and theora_comment structures using 
 *    theora_info_init() and theora_comment_init():
 \verbatim
 theora_info     info;
 theora_comment  comment;
   
 theora_info_init(&info);
 theora_comment_init(&comment);
 \endverbatim
 * -# retrieve header packets from ogg stream (there should be 3) and decode 
 *    into theora_info and theora_comment structures using 
 *    theora_decode_header().  See \ref identification for more information on 
 *    identifying which packets are theora packets.
 \verbatim
 int i;
 for (i = 0; i < 3; i++)
 {
   (get a theora packet "op" from the ogg stream)
   theora_decode_header(&info, &comment, op);
 }
 \endverbatim
 * -# initialise the decoder based on the information retrieved into the
 *    theora_info struct by theora_decode_header().  You will need a 
 *    theora_state struct.
 \verbatim
 theora_state state;
 
 theora_decode_init(&state, &info);
 \endverbatim
 * -# pass in packets and retrieve decoded frames!  See the yuv_buffer 
 *    documentation for information on how to retrieve raw YUV data.
 \verbatim
 yuf_buffer buffer;
 while (last packet was not e_o_s) {
   (get a theora packet "op" from the ogg stream)
   theora_decode_packetin(&state, op);
   theora_decode_YUVout(&state, &buffer);
 }
 \endverbatim
 *  
 *
 * \subsection identification Identifying Theora Packets
 *
 * all streams inside an ogg file have a unique serial_no attached to the 
 * stream.  Typically, you will want to 
 *  - retrieve the serial_no for each b_o_s (beginning of stream) page 
 *    encountered within the pgg file; 
 *  - test the first (only) packet on that page to determine if it is a theora 
 *    packet;
 *  - once you have found a theora b_o_s page then use the retrieved serial_no 
 *    to identify future packets belonging to the same theora stream.
 * 
 * Note that you \e cannot use theora_packet_isheader() to determine if a 
 * packet is a theora packet or not, as this function does not perform any
 * checking beyond whether a header bit is present.  Instead, use the
 * theora_decode_header() function and check the return value; or examine the
 * header bytes at the beginning of the ogg page.
 *
 * \subsection example Example Decoder 
 *
 * see <a href="http://svn.xiph.org/trunk/theora/examples/dump_video.c">
 * examples/dump_video.c</a> for a simple decoder implementation.
 *
 * \section encoding Encoding Process
 *
 * see <a href="http://svn.xiph.org/trunk/theora/examples/encoder_example.c">
 * examples/encoder_example.c</a> for a simple encoder implementation.
 */

/** \file
 * The libtheora C API.
 */

/**
 * A YUV buffer for passing uncompressed frames to and from the codec.
 * This holds a Y'CbCr frame in planar format. The CbCr planes can be
 * subsampled and have their own separate dimensions and row stride
 * offsets. Note that the strides may be negative in some 
 * configurations. For theora the width and height of the largest plane
 * must be a multiple of 16. The actual meaningful picture size and 
 * offset are stored in the theora_info structure; frames returned by
 * the decoder may need to be cropped for display.
 *
 * All samples are 8 bits. Within each plane samples are ordered by
 * row from the top of the frame to the bottom. Within each row samples
 * are ordered from left to right.
 *
 * During decode, the yuv_buffer struct is allocated by the user, but all
 * fields (including luma and chroma pointers) are filled by the library.  
 * These pointers address library-internal memory and their contents should 
 * not be modified.
 *
 * Conversely, during encode the user allocates the struct and fills out all
 * fields.  The user also manages the data addressed by the luma and chroma
 * pointers.  See the encoder_example.c and dump_video.c example files in
 * theora/examples/ for more information.
 */
typedef struct {
    int   y_width;      /**< Width of the Y' luminance plane */
    int   y_height;     /**< Height of the luminance plane */
    int   y_stride;     /**< Offset in bytes between successive rows */

    int   uv_width;     /**< Height of the Cb and Cr chroma planes */
    int   uv_height;    /**< Width of the chroma planes */
    int   uv_stride;    /**< Offset between successive chroma rows */
    unsigned char *y;   /**< Pointer to start of luminance data */
    unsigned char *u;   /**< Pointer to start of Cb data */
    unsigned char *v;   /**< Pointer to start of Cr data */

} yuv_buffer;

/**
 * A Colorspace.
 */
typedef enum {
  OC_CS_UNSPECIFIED,    /**< The colorspace is unknown or unspecified */
  OC_CS_ITU_REC_470M,   /**< This is the best option for 'NTSC' content */
  OC_CS_ITU_REC_470BG,  /**< This is the best option for 'PAL' content */
  OC_CS_NSPACES         /**< This marks the end of the defined colorspaces */
} theora_colorspace;

/**
 * A Chroma subsampling
 *
 * These enumerate the available chroma subsampling options supported
 * by the theora format. See Section 4.4 of the specification for
 * exact definitions.
 */
typedef enum {
  OC_PF_420,    /**< Chroma subsampling by 2 in each direction (4:2:0) */
  OC_PF_RSVD,   /**< Reserved value */
  OC_PF_422,    /**< Horizonatal chroma subsampling by 2 (4:2:2) */
  OC_PF_444,    /**< No chroma subsampling at all (4:4:4) */
} theora_pixelformat;

/**
 * Theora bitstream info.
 * Contains the basic playback parameters for a stream,
 * corresponds to the initial 'info' header packet.
 * 
 * Encoded theora frames must be a multiple of 16 is size;
 * this is what the width and height members represent. To
 * handle other sizes, a crop rectangle is specified in 
 * frame_height and frame_width, offset_x and offset_y. The
 * offset and size should still be a multiple of 2 to avoid
 * chroma sampling shifts. Offset values in this structure
 * are measured from the  upper left of the image.
 *
 * Frame rate, in frames per second, is stored as a rational
 * fraction. So is the aspect ratio. Note that this refers
 * to the aspect ratio of the frame pixels, not of the
 * overall frame itself.
 * 
 * see the example code for use of the other parameters and
 * good default settings for the encoder parameters.
 */
typedef struct {
  ogg_uint32_t  width;		/**< encoded frame width  */
  ogg_uint32_t  height;		/**< encoded frame height */
  ogg_uint32_t  frame_width;	/**< display frame width  */
  ogg_uint32_t  frame_height;	/**< display frame height */
  ogg_uint32_t  offset_x;	/**< horizontal offset of the displayed frame */
  ogg_uint32_t  offset_y;	/**< vertical offset of the displayed frame */
  ogg_uint32_t  fps_numerator;	    /**< frame rate numerator **/
  ogg_uint32_t  fps_denominator;    /**< frame rate denominator **/
  ogg_uint32_t  aspect_numerator;   /**< pixel aspect ratio numerator */
  ogg_uint32_t  aspect_denominator; /**< pixel aspect ratio denominator */
  theora_colorspace colorspace;	    /**< colorspace */
  int           target_bitrate;	    /**< nominal bitrate in bits per second */
  int           quality;  /**< Nominal quality setting, 0-63 */
  int           quick_p;  /**< Quick encode/decode */

  /* decode only */
  unsigned char version_major;
  unsigned char version_minor;
  unsigned char version_subminor;

  void *codec_setup;

  /* encode only */
  int           dropframes_p;
  int           keyframe_auto_p;
  ogg_uint32_t  keyframe_frequency;
  ogg_uint32_t  keyframe_frequency_force;  /* also used for decode init to
                                              get granpos shift correct */
  ogg_uint32_t  keyframe_data_target_bitrate;
  ogg_int32_t   keyframe_auto_threshold;
  ogg_uint32_t  keyframe_mindistance;
  ogg_int32_t   noise_sensitivity;
  ogg_int32_t   sharpness;

  theora_pixelformat pixelformat;	/**< chroma subsampling mode to expect */

} theora_info;

/** Codec internal state and context.
 */
typedef struct{
  theora_info *i;
  ogg_int64_t granulepos;

  void *internal_encode;
  void *internal_decode;

} theora_state;

/** 
 * Comment header metadata.
 *
 * This structure holds the in-stream metadata corresponding to
 * the 'comment' header packet.
 *
 * Meta data is stored as a series of (tag, value) pairs, in
 * length-encoded string vectors. The first occurence of the 
 * '=' character delimits the tag and value. A particular tag
 * may occur more than once. The character set encoding for
 * the strings is always utf-8, but the tag names are limited
 * to case-insensitive ascii. See the spec for details.
 *
 * In filling in this structure, theora_decode_header() will
 * null-terminate the user_comment strings for safety. However,
 * the bitstream format itself treats them as 8-bit clean,
 * and so the length array should be treated as authoritative
 * for their length.
 */
typedef struct theora_comment{
  char **user_comments;         /**< An array of comment string vectors */
  int   *comment_lengths;       /**< An array of corresponding string vector lengths in bytes */
  int    comments;              /**< The total number of comment string vectors */
  char  *vendor;                /**< The vendor string identifying the encoder, null terminated */

} theora_comment;

#define OC_FAULT       -1       /**< General failure */
#define OC_EINVAL      -10      /**< Library encountered invalid internal data */
#define OC_DISABLED    -11      /**< Requested action is disabled */
#define OC_BADHEADER   -20      /**< Header packet was corrupt/invalid */
#define OC_NOTFORMAT   -21      /**< Packet is not a theora packet */
#define OC_VERSION     -22      /**< Bitstream version is not handled */
#define OC_IMPL        -23      /**< Feature or action not implemented */
#define OC_BADPACKET   -24      /**< Packet is corrupt */
#define OC_NEWPACKET   -25      /**< Packet is an (ignorable) unhandled extension */
#define OC_DUPFRAME    1        /**< Packet is a dropped frame */


extern const char* __cdecl theora_version_string(void);
extern ogg_uint32_t __cdecl theora_version_number(void);
extern int __cdecl theora_encode_init(theora_state *th, theora_info *c);
extern int __cdecl theora_encode_YUVin(theora_state *t, yuv_buffer *yuv);
extern int __cdecl theora_encode_packetout( theora_state *t, int last_p,
                                    ogg_packet *op);
extern int __cdecl theora_encode_header(theora_state *t, ogg_packet *op);
extern int __cdecl theora_encode_comment(theora_comment *tc, ogg_packet *op);
extern int __cdecl theora_encode_tables(theora_state *t, ogg_packet *op);
extern int __cdecl theora_decode_header(theora_info *ci, theora_comment *cc,
                                ogg_packet *op);
extern int __cdecl theora_decode_init(theora_state *th, theora_info *c);
extern int __cdecl theora_decode_packetin(theora_state *th,ogg_packet *op);
extern int __cdecl theora_decode_YUVout(theora_state *th,yuv_buffer *yuv);
extern double __cdecl theora_granule_time(theora_state *th,ogg_int64_t granulepos);
extern void __cdecl theora_info_init(theora_info *c);
extern void __cdecl theora_info_clear(theora_info *c);
extern void __cdecl theora_clear(theora_state *t);

extern void __cdecl theora_comment_init(theora_comment *tc);
extern void __cdecl theora_comment_add(theora_comment *tc, char *comment);
extern void __cdecl theora_comment_add_tag(theora_comment *tc,
                                       char *tag, char *value);
extern char *__cdecl theora_comment_query(theora_comment *tc, char *tag, int count);
extern int   __cdecl theora_comment_query_count(theora_comment *tc, char *tag);
extern void  __cdecl theora_comment_clear(theora_comment *tc);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _O_THEORA_H_ */
