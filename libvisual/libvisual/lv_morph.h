/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _LV_MORPH_H
#define _LV_MORPH_H

#include <libvisual/lv_audio.h>
#include <libvisual/lv_palette.h>
#include <libvisual/lv_plugin.h>
#include <libvisual/lv_video.h>
#include <libvisual/lv_time.h>

/**
 * @defgroup VisMorph VisMorph
 * @{
 */

#ifdef __cplusplus

#include <libvisual/lv_intrusive_ptr.hpp>
#include <memory>
#include <string_view>

namespace LV {

  class Morph;

  typedef LV::IntrusivePtr<Morph> MorphPtr;

  //! Morph class
  class LV_API Morph
  {
  public:

      /**
       * Determines if a morph plugin by the given name is available.
       *
       * @param name Name of morph plugin to check for
       *
       * @return True if a morph plugin by that name is available, else false
       */
      static bool available (std::string_view name);

      /**
       * Creates a new Morph wit a plugin of a given name.
       *
       * @see realize()
       *
       * @param name Name of plugin to load
       *
       * @return New morph, or nullptr if plugin failed to load
       */
      static MorphPtr load (std::string_view name);

      Morph (Morph const& morph) = delete;

      ~Morph ();

      Morph& operator= (Morph const&) = delete;

      /**
       * Returns the plugin object.
       *
       * @return Plugin object
       */
      VisPluginData* get_plugin ();

      /**
       * Realizes a morph.
       *
       * This also calls the plugin init() method.
       *
       * @return true on success, false otherwise
       */
      bool realize ();

      /**
       * Returns the set of colour depths natively supported.
       *
       * @return Set of supported depths
       */
      VisVideoDepth get_supported_depths ();

      VisVideoAttrOptions const* get_video_attribute_options ();

      /**
       * Sets the video target for rendering.
       *
       * @param video Video target
       */
      void set_video (VideoPtr const& video);

      /**
       * Sets the morph duration.
       *
       * @param time Duration
       */
      void set_time (Time const& time);

      /**
       * Sets the progress of a morph.
       *
       * @param progress Progress of morph (must be in [0.0, 1.0])
       */
      void set_progress (float progress);

      /**
       * Returns the colour palette used.
       *
       * @note Only 8-bit morphs will have colour palettes.
       *
       * @return Colour palette, or nullptr if there is none
       */
      Palette const* get_palette ();

      /**
       * Determines if morph has completed.
       *
       * @return true if completed, false otherwise
       */
      bool is_done ();

      /**
       * Runs the morph.
       *
       * @note The rendering will be performed on the Video set with set_video().
       *
       * @param audio Audio data
       * @param src1  First source Video
       * @param src2  Second source Video
       *
       * @return true on success, false otherwise
       */
      bool run (Audio const& audio, VideoPtr const& src1, VideoPtr const& src2);

private:

      friend void intrusive_ptr_add_ref (Morph const* morph);
      friend void intrusive_ptr_release (Morph const* morph);

      class Impl;
      const std::unique_ptr<Impl> m_impl;

      mutable unsigned int m_ref_count;

      explicit Morph (std::string_view name);
  };

  inline void intrusive_ptr_add_ref (Morph const* morph)
  {
      morph->m_ref_count++;
  }

  inline void intrusive_ptr_release (Morph const* morph)
  {
      if (--morph->m_ref_count == 0) {
          delete morph;
      }
  }

} // LV namespace

typedef LV::Morph VisMorph;

#else

typedef struct _VisMorph VisMorph;
struct _VisMorph;

#endif

typedef struct _VisMorphPlugin VisMorphPlugin;

/**
 * Function signature and type of the Morph palette() method.
 *
 * The palette() method is called to obtain the colour palette used in 8-bit rendering modes.
 *
 * @param plugin   Plugin object
 * @param progress Morph progress (must be in [0.0, 1.0])
 * @param audio    Audio data
 * @param palette  Palette generated by LV
 * @param src1     First source Video
 * @param src2     Second source Video
 */
typedef void (*VisPluginMorphPaletteFunc) (VisPluginData *plugin,
                                           float          progress,
                                           VisAudio      *audio,
                                           VisPalette    *palette,
                                           VisVideo      *src1,
                                           VisVideo      *src2);

/**
 * Function signature and type of the Morph apply() method.
 *
 * The apply() method is called to render the morph of two Actor renders.
 *
 * @param plugin   Plugin object
 * @param progress Morph progress (must be in [0.0 and 1])
 * @param audio    Audio data
 * @param dest     Destination video
 * @param src1     First source Video
 * @param src2     Second source Video
 */
typedef void (*VisPluginMorphApplyFunc) (VisPluginData *plugin,
                                         float          progress,
                                         VisAudio      *audio,
                                         VisVideo      *dest,
                                         VisVideo      *src1,
                                         VisVideo      *src2);

/**
 * Morph plugin class.
 */
struct _VisMorphPlugin {
    VisPluginMorphPaletteFunc palette;        /**< Returns the color palette (only for 8-bit morphs) */
    VisPluginMorphApplyFunc   apply;          /**< Renders the morph */
    int                       requests_audio; /**< Flag indicating if morph plugin uses audio data */
    VisVideoAttrOptions       vidoptions;
};

LV_BEGIN_DECLS

/**
 * Returns the name of the next available morph plugin.
 *
 * @see visual_morph_get_prev_by_name()
 *
 * @param name Name of the current plugin, or NULL to retrieve the first.
 *
 * @return The name of the next plugin within the list.
 */
LV_API const char *visual_morph_get_next_by_name (const char *name);

/**
 * Returns the name of the previous available morph plugin.
 *
 * @see visual_morph_get_next_by_name()
 *
 * @param name Name of the current plugin. or NULL to retrieve the last.
 *
 * @return The name of the previous plugin within the list.
 */
LV_API const char *visual_morph_get_prev_by_name (const char *name);

LV_NODISCARD LV_API VisMorph *visual_morph_new (const char *name);

LV_API void visual_morph_ref   (VisMorph *morph);
LV_API void visual_morph_unref (VisMorph *morph);

LV_API VisPluginData       *visual_morph_get_plugin                  (VisMorph *morph);
LV_API VisVideoDepth        visual_morph_get_supported_depths        (VisMorph *morph);
LV_API VisVideoAttrOptions *visual_morph_get_video_attribute_options (VisMorph *morph);
LV_API int                  visual_morph_requests_audio              (VisMorph *morph);

LV_API void visual_morph_set_video    (VisMorph *morph, VisVideo *video);
LV_API void visual_morph_set_time     (VisMorph *morph, VisTime *time);
LV_API void visual_morph_set_progress (VisMorph *morph, float progress);

LV_API int visual_morph_realize (VisMorph *morph);
LV_API int visual_morph_run     (VisMorph *morph, VisAudio *audio, VisVideo *src1, VisVideo *src2);
LV_API int visual_morph_is_done (VisMorph *morph);

LV_API VisPalette *visual_morph_get_palette (VisMorph *morph);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_MORPH_H */