"""Regression test for the fx-CG50 bytearray behavior seen on real hardware."""

import os
import sys
import unittest


HERE = os.path.dirname(__file__)
sys.path.insert(0, HERE)

# Reuse the fake gint environment from the main compatibility test.
import pygame_compat as compat

pygame = compat.pygame


class CG50SurfaceRegressionTests(unittest.TestCase):
    def test_convert_alpha_copies_pixels_without_slice_assignment(self):
        source = pygame.Surface((3, 2))
        source.fill((12, 34, 56))
        source.set_at((1, 1), (240, 120, 16))

        converted = source.convert_alpha()
        self.assertIsNot(converted, source)
        self.assertEqual(converted.get_size(), source.get_size())
        self.assertGreater(converted.get_at((1, 1)).r, 230)
        self.assertTrue(converted.get_flags() & pygame.SRCALPHA)

    def test_patched_source_avoids_bytearray_slice_copy(self):
        module_path = os.path.abspath(os.path.join(
            HERE, "../../../ports/sh/modules/pygame/__init__.py"))
        with open(module_path, "r", encoding="utf-8") as handle:
            source = handle.read()
        self.assertNotIn("result._data[:] = self._data", source)
        self.assertNotIn("self._data[start:start + len(row)] = row", source)


if __name__ == "__main__":
    unittest.main()
