import { Canvas, useFrame, useThree } from '@react-three/fiber';
import * as THREE from 'three';
import React, { useRef, useEffect } from 'react';

function AnimatedGradient() {
  const shaderRef = useRef<THREE.ShaderMaterial>(null);
  const { size } = useThree();

  // Update time every frame
  useFrame(({ clock }) => {
    if (shaderRef.current) {
      shaderRef.current.uniforms.uTime.value = clock.getElapsedTime();
    }
  });
  useFrame(({ pointer }) => {
    if(shaderRef.current) {
      shaderRef.current.uniforms.uPointer.value = pointer;
    }
  })

  // Update resolution on resize
  useEffect(() => {
    if (shaderRef.current) {
      shaderRef.current.uniforms.uResolution.value.set(size.width, size.height);
    }
  }, [size]);

  return (
    <mesh>
      <planeGeometry args={[2, 2]} />
      <shaderMaterial
        ref={shaderRef}
        uniforms={{
          uTime: { value: 0 },
          uResolution: { value: new THREE.Vector2(size.width, size.height) },
          uPointer : {value : new THREE.Vector2(size.width/2, size.height/2)},
          decayRate : {value : 0.5}
        }}
        vertexShader={/* glsl */`
             varying vec2 vUv;
              void main() {
                vUv = uv;
                gl_Position = vec4(position, 1.0);
              }
        `}
        fragmentShader={/* glsl */`
          precision mediump float;
    uniform vec2 uResolution;
    uniform float uTime;
    varying vec2 vUv;

    void main() {
      vec2 uv = vUv;
      
      // --- Gradient background ---
      vec3 topColor = vec3(0.02, 0.02, 0.03);
      vec3 bottomColor = vec3(0.1, 0.0, 0.2);
      vec3 bg = mix(topColor, bottomColor, pow(uv.y, 1.5));
      
      // --- Grid dots ---
      vec2 grid = uv * uResolution / 15.0;
      vec2 g = fract(grid) - 0.5;
      float d = length(g);
      float dot = smoothstep(0.25, 0.15, d);
      dot *= 0.8 + 0.2 * sin(uTime * 2.0 + grid.x * 0.5);
      
      // --- Combine grid + gradient ---
      vec3 color = bg + vec3(dot * 0.08);
      
      // --- Bottom glow ---
      float glow = smoothstep(0.0, 0.5, 1.0 - uv.y);
      glow *= exp(-pow(1.0 - uv.y, 3.0) * 8.0);
      color += vec3(0.1, 0.0, 0.3) * glow * 0.8;
      
      gl_FragColor = vec4(color, 1.0);
    }
        `}
        depthWrite={false}
      />
    </mesh>
  );
}

interface MyCanvasProps {
  className?: string;
}

const ThreeCanvas = ({ className = "" }: MyCanvasProps) => (
  <Canvas
    className={`absolute inset-0 z-0 ${className}`}
    camera={{ position: [0, 0, 1], near: 0.1, far: 10 }}
  >
    <AnimatedGradient />
  </Canvas>
);

export default ThreeCanvas;
