#include <Game.hpp>
#include <../Engine/include/Globals.hpp>
#include <GameObject.hpp>
#include <CompilingOptions.hpp>
#include <MathsUtils.hpp>
#include <Audio.hpp>
#include <Constants.hpp>

#include <thread>
#include <fstream>

#include <Helpers.hpp>
#include <VulpineAssets.hpp>
#include <Skeleton.hpp>

Game::Game(GLFWwindow *window) : App(window) {}

void Game::init(int paramSample)
{
    App::init();

    // activateMainSceneBindlessTextures();
    activateMainSceneClusteredLighting(ivec3(16, 9, 24), 5e3f);

    setIcon("ressources/icon.png");

    setController(&spectator);

    ambientLight = vec3(0.1);

    finalProcessingStage = ShaderProgram(
        "shader/post-process/final composing.frag",
        "shader/post-process/basic.vert",
        "",
        globals.standartShaderUniform2D());

    finalProcessingStage.addUniform(ShaderUniform(Bloom.getIsEnableAddr(), 10));

    camera.init(radians(70.0f), globals.windowWidth(), globals.windowHeight(), 0.1f, 1E4f);
    // camera.setMouseFollow(false);
    // camera.setPosition(vec3(0, 1, 0));
    // camera.setDirection(vec3(1, 0, 0));
    auto myfile = std::fstream("saves/cameraState.bin", std::ios::in | std::ios::binary);
    if(myfile)
    {
        CameraState buff;
        myfile.read((char*)&buff, sizeof(CameraState));
        myfile.close();
        camera.setState(buff);
    }


    /* Loading 3D Materials */
    depthOnlyMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/depthOnly.frag",
            "shader/foward/basic.vert",
            ""));

    depthOnlyStencilMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/depthOnlyStencil.frag",
            "shader/foward/basic.vert",
            ""));

    depthOnlyInstancedMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/depthOnlyStencil.frag",
            "shader/foward/basicInstance.vert",
            ""));

    depthOnlyAnimatedMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/depthOnlyStencil.frag",
            "shader/animation/animated.vert",
            ""));

    GameGlobals::PBR = MeshMaterial(
        new ShaderProgram(
            "shader/foward/PBR.frag",
            // "shader/clustered/clusterDebug.frag",
            "shader/foward/basic.vert",
            "",
            globals.standartShaderUniform3D()));

    GameGlobals::PBRstencil = MeshMaterial(
        new ShaderProgram(
            "shader/foward/PBR.frag",
            "shader/foward/basic.vert",
            "",
            globals.standartShaderUniform3D()));

    GameGlobals::PBRinstanced = MeshMaterial(
        new ShaderProgram(
            "shader/foward/PBR.frag",
            "shader/foward/basicInstance.vert",
            "",
            globals.standartShaderUniform3D()));

    GameGlobals::PBRanimated = MeshMaterial(
        new ShaderProgram(
            "shader/foward/PBR.frag",
            "shader/animation/animated.vert",
            "",
            globals.standartShaderUniform3D()));

    skyboxMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/foward/Skybox.frag",
            "shader/foward/basic.vert",
            "",
            globals.standartShaderUniform3D()));

    GameGlobals::PBRstencil.depthOnly = depthOnlyStencilMaterial;
    GameGlobals::PBRanimated.depthOnly = depthOnlyAnimatedMaterial;
    GameGlobals::PBRinstanced.depthOnly = depthOnlyInstancedMaterial;
    scene.depthOnlyMaterial = depthOnlyMaterial;

    /* UI */
    FUIfont = FontRef(new FontUFT8);
    FUIfont->readCSV("ressources/fonts/Roboto/out.csv");
    FUIfont->setAtlas(Texture2D().loadFromFileKTX("ressources/fonts/Roboto/out.ktx"));
    defaultFontMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/2D/sprite.frag",
            "shader/2D/sprite.vert",
            "",
            globals.standartShaderUniform2D()));

    defaultSUIMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/2D/fastui.frag",
            "shader/2D/fastui.vert",
            "",
            globals.standartShaderUniform2D()));

    fuiBatch = SimpleUiTileBatchRef(new SimpleUiTileBatch);
    fuiBatch->setMaterial(defaultSUIMaterial);
    fuiBatch->state.position.z = 0.0;
    fuiBatch->state.forceUpdate();

    /* VSYNC and fps limit */
    globals.fpsLimiter.activate();
    globals.fpsLimiter.freq = 144.f;
    glfwSwapInterval(0);
}

bool Game::userInput(GLFWKeyInfo input)
{
    if (baseInput(input))
        return true;

    if (input.action == GLFW_PRESS)
    {
        switch (input.key)
        {
        case GLFW_KEY_ESCAPE:
            state = quit;
            break;

        case GLFW_KEY_F2:
            globals.currentCamera->toggleMouseFollow();
            break;

        case GLFW_KEY_1:
            Bloom.toggle();
            break;

        case GLFW_KEY_2:
            SSAO.toggle();
            break;

        case GLFW_KEY_F5:
            #ifdef _WIN32
            system("cls");
            #else
            system("clear");
            #endif

            finalProcessingStage.reset();
            Bloom.getShader().reset();
            SSAO.getShader().reset();
            depthOnlyMaterial->reset();
            GameGlobals::PBR->reset();
            GameGlobals::PBRinstanced->reset();
            GameGlobals::PBRstencil->reset();
            GameGlobals::PBRanimated->reset();
            
            GameGlobals::PBRinstanced.depthOnly->reset();
            GameGlobals::PBRstencil.depthOnly->reset();
            GameGlobals::PBRanimated.depthOnly->reset();
            skyboxMaterial->reset();
            break;
        
        case GLFW_KEY_F6:
            if(helpers->state.hide == ModelStateHideStatus::SHOW)
                helpers->state.hide = ModelStateHideStatus::HIDE;
            else
                helpers->state.hide = ModelStateHideStatus::SHOW;
            break;

        case GLFW_KEY_F8:
            {
                auto myfile = std::fstream("saves/cameraState.bin", std::ios::out | std::ios::binary);
                myfile.write((char*)&camera.getState(), sizeof(CameraState));
                myfile.close();
            }
                break;

        default:
            break;
        }
    }

    return true;
};

void Game::physicsLoop()
{
    physicsTicks.freq = 45.f;
    physicsTicks.activate();

    while (state != quit)
    {
        physicsTicks.start();

        physicsMutex.lock();
        physicsEngine.update(1.f / physicsTicks.freq);
        physicsMutex.unlock();

        physicsTicks.waitForEnd();
    }
}

void Game::mainloop()
{
    /* Loading Models and setting up the scene */
    ModelRef skybox = newModel(skyboxMaterial);
    skybox->loadFromFolder("ressources/models/skybox/", true, false);

    // skybox->invertFaces = true;
    skybox->depthWrite = true;
    skybox->state.frustumCulled = false;
    skybox->state.scaleScalar(1E6);
    scene.add(skybox);

    ModelRef floor = newModel(GameGlobals::PBR);
    floor->loadFromFolder("ressources/models/cube/");

    int gridSize = 10;
    int gridScale = 7.5;
    for (int i = -gridSize; i <= gridSize; i++)
        for (int j = -gridSize; j <= gridSize; j++)
        {
            ModelRef f = floor->copyWithSharedMesh();
            f->state
                .setScale(vec3(gridScale, 0.25f, gridScale))
                .setPosition(vec3(i * gridScale * 2.0, 0.0, j * gridScale * 2.0));
            scene.add(f);
        }

    // int forestSize = 14;
    // float treeScale = 0.20;

    // ModelRef leaves = newModel(GameGlobals::PBRstencil);
    // leaves->loadFromFolder("ressources/models/fantasy tree/");
    // leaves->noBackFaceCulling = true;

    // ModelRef trunk = newModel(GameGlobals::PBR);
    // trunk->loadFromFolder("ressources/models/fantasy tree/trunk/");

    // for (int i = -forestSize; i < forestSize; i++)
    //     for (int j = -forestSize; j < forestSize; j++)
    //     {
    //         ObjectGroupRef tree = newObjectGroup();
    //         tree->add(trunk->copyWithSharedMesh());
    //         ModelRef l = leaves->copyWithSharedMesh();
    //         l->noBackFaceCulling = true;
    //         tree->add(l);
    //         tree->state
    //             .scaleScalar(treeScale)
    //             .setPosition(vec3(i * treeScale * 50, 0, j * treeScale * 50));

    //         scene.add(tree);
    //     }

    /* Instanced Mesh example */
    // InstancedModelRef trunk = newInstancedModel();
    // trunk->setMaterial(GameGlobals::PBRinstanced);
    // trunk->loadFromFolder("ressources/models/fantasy tree/trunk/");
    // trunk->allocate(2E4);

    // for(int i = -forestSize; i < forestSize; i++)
    // for(int j = -forestSize; j < forestSize; j++)
    // {
    //     ModelInstance &inst = *trunk->createInstance();
    //     inst.scaleScalar(treeScale)
    //         .setPosition(vec3(i*treeScale*40, 0, j*treeScale*40));
    //     inst.update();
    // }
    // trunk->updateInstances();
    // scene.add(trunk);

    SceneDirectionalLight sun = newDirectionLight(
        DirectionLight()
            .setColor(vec3(143, 107, 71) / vec3(255))
            .setDirection(normalize(vec3(-1.0, -1.0, 0.0)))
            .setIntensity(1.0));

    sun->cameraResolution = vec2(2048);
    sun->shadowCameraSize = vec2(90, 90);
    sun->activateShadows();
    scene.add(sun);

    ObjectGroupRef lights = newObjectGroup();
    helpers = newObjectGroup();
    int nbLights = 0;
    for(int i = 0; i < nbLights; i++)
    {
        ScenePointLight l = newPointLight();

        float phi = (float)(std::rand()%3141592)/(PI);
        const float maxDist = 300.f;
        float dist = (float)(std::rand()%(int)1e5)/1e5f;
        dist = pow(dist, 0.5f)*maxDist;

        l->setIntensity(5.f)
            .setRadius(10.0)
            .setColor(hsv2rgb(vec3((float)(std::rand()%256)/256.f, 1.0, 1.f)))
            .setPosition(dist*PhiThetaToDir(vec2(phi, 0)) + vec3(0, l->radius()*0.25, 0));

        lights->add(l);
        helpers->add(PointLightHelperRef(new PointLightHelper(l)));
    }
    scene.add(lights);
    helpers->state.hide = ModelStateHideStatus::HIDE;
    scene.add(helpers);

    // scene.add(ClusteredFrustumHelperRef(new ClusteredFrustumHelper(camera)));

    // scene.add(newPointLight(
    //     PointLight().setColor(vec3(1)).setRadius(40)
    // ));

    scene.activateClusteredLighting();


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.0);

    /* Setting up the UI */
    FastUI_context ui(fuiBatch, FUIfont, scene2D, defaultFontMaterial);
    FastUI_valueMenu menu(ui, {});

    menu->state.setPosition(vec3(-0.9, 0.5, 0)).scaleScalar(0.8);
    globals.appTime.setMenuConst(menu);
    globals.cpuTime.setMenu(menu);
    globals.gpuTime.setMenu(menu);
    globals.fpsLimiter.setMenu(menu);

    // physicsTicks.setMenu(menu);
    // sun->setMenu(menu, U"Sun");

    BenchTimer cullTimer("Light Culling");
    cullTimer.setMenu(menu);

    menu.batch();
    scene2D.updateAllObjects();
    fuiBatch->batch();

    state = AppState::run;
    std::thread physicsThreads(&Game::physicsLoop, this);

    glLineWidth(5.0);

    MeshVao test = loadVulpineMesh("ressources/models/vulpineMesh/Beta_Surface.vulpineMesh");
    ModelRef testmodel = newModel(GameGlobals::PBRanimated, test);
    testmodel->setMap(Texture2D().loadFromFileKTX("ressources/models/vulpineMesh/CE.ktx"), 0);
    testmodel->setMap(Texture2D().loadFromFileKTX("ressources/models/vulpineMesh/NRM.ktx"), 1);
    testmodel->state.scaleScalar(1);
    scene.add(testmodel);


    SkeletonRef humanSkeleton(new Skeleton);
    humanSkeleton->load("ressources/models/animations/human.vulpineSkeleton");

    SkeletonAnimationState dummyState;
    dummyState.skeleton = humanSkeleton;
    for(int i = 0; i < humanSkeleton->getSize(); i++) dummyState.push_back(mat4(1));
    humanSkeleton->applyGraph(dummyState);
    dummyState.send();

    SkeletonAnimationState defaultState;
    for(int i = 0; i < humanSkeleton->getSize(); i++) defaultState.push_back(mat4(1));
    humanSkeleton->applyGraph(defaultState);
    defaultState.send();


    SkeletonHelperRef animHelper(new SkeletonHelper(dummyState));
    scene.add(animHelper);

    /* Main Loop */
    while (state != AppState::quit)
    {
        mainloopStartRoutine();

        for (GLFWKeyInfo input; inputs.pull(input); userInput(input));

        menu.trackCursor();
        menu.updateText();

        mainloopPreRenderRoutine();

        float time = globals.simulationTime.getElapsedTime();
        lights->state.setRotation(vec3(0, time*0.25, 0));


        for(int i = 0; i < humanSkeleton->getSize(); i++) dummyState[i] = mat4(1);
        ModelState3D dummyBone;
        // dummyBone.setPosition(vec3(0, 1.0 + cos(time), 0));
        dummyBone.setRotation(vec3(0.5*cos(time), 0, 0));
        dummyBone.update();
        // id 12 : left shoulder
        dummyState[12] = dummyBone.modelMatrix;
        for(int i = 2; i < humanSkeleton->getSize(); i++) dummyState[i] = dummyBone.modelMatrix;
        humanSkeleton->applyGraph(dummyState);

        dummyState.update();
        dummyState.activate(2);
        defaultState.activate(3);

        /* UI & 2D Render */
        glEnable(GL_BLEND);
        glEnable(GL_FRAMEBUFFER_SRGB);

        scene2D.updateAllObjects();
        fuiBatch->batch();
        screenBuffer2D.activate();
        fuiBatch->draw();
        scene2D.cull();
        scene2D.draw();
        screenBuffer2D.deactivate();

        /* 3D Pre-Render */
        glDisable(GL_FRAMEBUFFER_SRGB);
        glDisable(GL_BLEND);
        glDepthFunc(GL_GREATER);
        glEnable(GL_DEPTH_TEST);

        scene.updateAllObjects();
        scene.generateShadowMaps();
        renderBuffer.activate();

        
        scene.cull();
        

        /* 3D Early Depth Testing */
        scene.depthOnlyDraw(*globals.currentCamera, true);
        glDepthFunc(GL_EQUAL);

        /* 3D Render */
        cullTimer.start();
        scene.genLightBuffer();
        cullTimer.end();
        skybox->bindMap(0, 4);
        scene.draw();
        renderBuffer.deactivate();

        /* Post Processing */
        renderBuffer.bindTextures();
        SSAO.render(*globals.currentCamera);
        Bloom.render(*globals.currentCamera);

        /* Final Screen Composition */
        glViewport(0, 0, globals.windowWidth(), globals.windowHeight());
        finalProcessingStage.activate();
        // sun->shadowMap.bindTexture(0, 6);
        screenBuffer2D.bindTexture(0, 7);
        globals.drawFullscreenQuad();

        /* Main loop End */
        mainloopEndRoutine();
    }

    physicsThreads.join();
}
