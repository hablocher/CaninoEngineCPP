# O Despertar da RHI: Pura Submissão Gráfica

A *Rendering Hardware Interface* (RHI) deixou o plano teórico e ativou nossa GPU através do *Native Window Handle*. Conforme a arquitetura DOD dita, não trouxemos a `libGL` pesada instanciando centenas de *Objects* de biblioteca standard para sujar nosso Engine Core. 

Usamos puro C-Style WGL Backend Opaque Bridge pra alcançar a aceleração de hardware. 

## Engenharia Implementada
1. **Quebra de Contenção Nativa (`PlatformGetNativeWindowHandle`)**: Realizamos o vazamento estruturado e controlado em `window_win32.cpp` devolvendo para a Camada RHI um `void*` (Que representa o `HWND` da Janela GDI do Windows Mapped Space).
2. **Setup OpenGL C-Style Sem Headers Externos (`opengl_rhi.cpp`)**: Rejeitamos o uso do _GLAD_ ou _GLEW_. Ligamos diretamente ao `opengl32.lib` exportado secretamente pelo próprio SO. Acionamos o *PixelFormatDescriptor* puro em Assembly/C invocando o Double Buffer e instanciando o `HGLRC`(Render Context).
3. **API Fronteira Segura (`rhi.h`)**: As chamadas `RenderCommand::SetClearColor` e `RenderCommand::Clear()` funcionam como despachos DOD abstratos. No futuro substituiremos pra preenchimento de *Ring Buffers* Vulkan sem o GameLoop nem piscar o olho.

## Feedback Visual (*Zero Crash Runtime*)

A Sandbox rodou nossa inicialização crítica de RHI e disparou o Loop Real-Time com êxito avassalador, conectando logicamente suas teclas pressionadas a variáveis na memória:

```text
[Sandbox] Iniciando Engine DOD e forjando Kernel Hardware Window...
[Sandbox] Render Hardware Interface Ativa. Real-time Infinity Loop inicializado.
```

(Isso significa que a Engine criou o Device Context, ligou o PFD pra GPU e o SwapBuffers ocorreu em real-time!).
As teclas (A, D, Space) agora controlam as mutações de RGB dinâmicas puramente integradas pelo Input DOD state pra repintar a tela nativamente! Nossos alicerces (Memory, OS Platform, Input e RHI Base) estão consolidados.
