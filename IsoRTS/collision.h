#pragma once
/*
 * File:   collision.h
 * Authors: Nick Koirala (original version), ahnonay (SFML2 compatibility)
 *
 * Collision Detection and handling class
 * For SFML2.

Notice from the original version:

(c) 2009 - LittleMonkey Ltd

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
   you must not claim that you wrote the original software.
   If you use this software in a product, an acknowledgment
   in the product documentation would be appreciated but
   is not required.

2. Altered source versions must be plainly marked as such,
   and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
   source distribution.

 *
 * Created on 30 January 2009, 11:02
 */

#include <SFML/Graphics.hpp>


namespace Collision {
	//////
	/// Test for a collision between two sprites by comparing the alpha values of overlapping pixels
	/// Supports scaling and rotation
	/// AlphaLimit: The threshold at which a pixel becomes "solid". If AlphaLimit is 127, a pixel with
	/// alpha value 128 will cause a collision and a pixel with alpha value 126 will not.
	/// 
	/// This functions creates bitmasks of the textures of the two sprites by
	/// downloading the textures from the graphics card to memory -> SLOW!
	/// You can avoid this by using the "CreateTextureAndBitmask" function
	//////
	bool PixelPerfectTest(const sf::Sprite& Object1, const sf::Sprite& Object2, sf::Uint8 AlphaLimit = 0);

	//////
	/// Replaces Texture::loadFromFile
	/// Load an imagefile into the given texture and create a bitmask for it
	/// This is much faster than creating the bitmask for a texture on the first run of "PixelPerfectTest"
	/// 
	/// The function returns false if the file could not be opened for some reason
	//////
	bool CreateTextureAndBitmask(sf::Texture& LoadInto, const std::string& Filename);

	//////
	/// Test for collision using circle collision dection
	/// Radius is averaged from the dimensions of the sprite so
	/// roughly circular objects will be much more accurate
	//////
	bool CircleTest(const sf::Sprite& Object1, const sf::Sprite& Object2);

	//////
	/// Test for bounding box collision using the Separating Axis Theorem
	/// Supports scaling and rotation
	//////
	bool BoundingBoxTest(const sf::Sprite& Object1, const sf::Sprite& Object2);
}



