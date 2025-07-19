import { Canvas, useFrame } from '@react-three/fiber';
import * as THREE from 'three';
import { useRef } from 'react';
import React from 'react';

function AnimatedGradient() {
    const shaderRef = React.useRef<any>();
    useFrame(({ clock }) => {
        if(shaderRef.current){
            shaderRef.current.uniforms.uTime.value = clock.getElapsedTime();
        }
    })

    return (
        <mesh>
            <planeGeometry args={[2, 2]} />
            <shaderMaterial 
             ref={shaderRef}
             uniforms={{
                uTime : {value : 0},
                radius : {value : 5},
             }}
             vertexShader={`
                void main()
                {
                    gl_Position = vec4(position, 1.0);
                }`}
             fragmentShader={`
                
                precision highp float;
                uniform float uTime;

              
                void main(){
                    vec2 coord = gl_FragCoord.xy;

                    float gridSize = 40.0;
                    vec2 snapped = floor(coord / gridSize) * gridSize;

                    bool isOnGrid = distance(coord, snapped) < 1.0;

                    float intensity = 0.5 + sin(uTime + snapped.x * 0.1 + snapped.y * 0.1);

                    vec3 onColor = vec3(0.7, 0.8, 0.7) * vec3(intensity);
                    vec3 offColor = vec3(0.05);

                    gl_FragColor = vec4(isOnGrid? onColor : offColor, 1.0);
                }
                `}
                depthWrite={false} />
        </mesh>
    )
}

interface MyCanvasProps {
    className? : string,
}

const ThreeCanvas = ({className} : MyCanvasProps) => {
    if(className === undefined){
        className = "";
    }
    return (<Canvas className={`absolute inset-0 z-0 ${className}`} camera={{ position : [0 ,0, 1], near : 0.1, far : 10}}>
        <AnimatedGradient />
    </Canvas>)
}

export default ThreeCanvas;