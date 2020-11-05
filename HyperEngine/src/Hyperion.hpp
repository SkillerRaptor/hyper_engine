#pragma once

/* Include File for HyperEngine Application / Projects */

/* Core */
#include "Hyperion/Core/Core.hpp"

/* Entity Component System */
#include "Hyperion/HyperECS/Components.hpp"
#include "Hyperion/HyperECS/Entity.hpp"
#include "Hyperion/HyperECS/EntitySystem.hpp"
#include "Hyperion/HyperECS/Registry.hpp"

/* Events */
#include "Hyperion/HyperEvents/Event.hpp"
#include "Hyperion/HyperEvents/KeyEvents.hpp"
#include "Hyperion/HyperEvents/MouseEvents.hpp"
#include "Hyperion/HyperEvents/WindowEvents.hpp"

/* Layers */
#include "Hyperion/HyperLayer/Layer.hpp"
#include "Hyperion/HyperLayer/OverlayLayer.hpp"

/* Maths */
#include "Hyperion/HyperMath/Matrix.hpp"
#include "Hyperion/HyperMath/Vector.hpp"
#include "Hyperion/HyperMath/Quaternion.hpp"

/* Memory */
#include "Hyperion/Memory/PoolAllocator.hpp"

/* Physics */

/* Rendering */
#include "Hyperion/HyperRendering/RenderContext.hpp"
#include "Hyperion/HyperRendering/ShaderManager.hpp"
#include "Hyperion/HyperRendering/TextureManager.hpp"

/* Utilities */
#include "Hyperion/Utilities/Hasher.hpp"
#include "Hyperion/Utilities/Input.hpp"
#include "Hyperion/Utilities/KeyCodes.hpp"
#include "Hyperion/Utilities/Log.hpp"
#include "Hyperion/Utilities/Logger.hpp"
#include "Hyperion/Utilities/MouseCodes.hpp"
#include "Hyperion/Utilities/NonCopyable.hpp"
#include "Hyperion/Utilities/NonMoveable.hpp"
#include "Hyperion/Utilities/Random.hpp"
#include "Hyperion/Utilities/Timer.hpp"
#include "Hyperion/Utilities/Timestep.hpp"
