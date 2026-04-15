# Math notes

So as not to clutter README, here's where I'll keep my notes on the theory.

## The approach to soft clipping

### Background
- hard clipping is simple because it just takes $\operatorname{clamp}(x, -1, 1)$.
  - clip visualization is easy - anything over 0db is clipping.
  - until the clipping threshold, the output is identity (i.e. for $0\leq x <1, f(x) = x$)

- soft clipping is complex - very nonlinear
  - clip visualization is hard. theoretically every output sample will differ from the input. visualize as gradient based on magnitude of (out - in)?
  - harder to reason about the impact clipping has on the signal; trickier to think about how to "dial in" for mix/master work
  - more precisely, compare a quiet (say, -12db or below) signal going through hard clipping and something like Oxford Inflator; in the former case, nothing changes, whereas in the latter case, the amplitude and thus loudness are substantially different

### Constraints

So, let's identify some conditions that will make for good soft clipping respecting the above. Let $f(x)$ be the saturation function and define $L>0$, the saturation point. 

0. $f(-x) = -f(x)$ (definition of odd function; $f$ is not asymmetrical and only adds odd harmonics)
1. $f'(x) > 0$ for $|x| < L$ ($f$ is increasing, except perhaps at and beyond the saturation points where it flattens to 0)
2. $f'(0) = 1$ ($f$ behaves like the identity in the neighborhood of 0)
3. $\operatorname{Image}(f) = [-1, 1]$ (the clipped signal is no greater than 0db)
4. $x|f''(x)| < 0$ (equivalently, $f(x)$ is concave down for $x>0$ and concave up for $x<0$; the saturation curve is sigmoid-shaped

To summarize, $f$ should be odd, look like a sigmoid or a familiar soft saturation curve, and fully saturate at and above $L$, but additionally behave like the identity function for quiet signals and exhibit more apparent saturation the more you push it. 

Conjuring images of such classes of functions ($\operatorname{erf}$, $\tanh$, smoothstep), and then thinking about what their derivatives look like (normal distribution, $\operatorname{sech}^2$, inverted parabola), one notices that the derivatives are generally bell curves. We can select bell curves that cap out at 1, are nonnegative, and similarly adhere to the desired properties, and then take an integral.

### Examples

Here are some examples I was able to come up with goofing around in desmos and skimming wikipedia articles.

0. Hard clip, for reference: ($L=1$, class $C^0$)

    $ f(x) 
    = \operatorname{clamp}(x, -1, 1) 
    = \begin{cases}
    -1  & x<-1 \\ 
    x   & -1 \leq x \leq 1 \\ 
    1   & 1<x 
    \end{cases}$

1. A construction using sine ($L=\pi/2$, class $C^1$)

    $ f(x) 
    = \begin{cases}
    -1      & x<-\frac{\pi}{2} \\
    \sin(x) & -\frac{\pi}{2} \leq x \leq \frac{\pi}{2} \\
    1       & \frac{\pi}{2}<x
    \end{cases}
    $

2. Hyperbolic tangent ($L$ is at infinity, class $C^{\infty}$)

    $ f(x) = \tanh(x)$

3. Error function (ditto)

    $ f(x) = \operatorname{erf}(\frac{x\sqrt{\pi}}{2})$

4. The "Cambridge Bloater" (based on a plugin hated for its iLok requirement. $L=2$, class $C^1$)

    $ f(x)
    = \begin{cases}
    -1  & x<-2 \\ 
    x+\frac{x^2}{4} & -2 \leq x \leq 0 \\
    x-\frac{x^2}{4} & 0 < x \leq 2 \\
    1   & 2<x 
    \end{cases}
    $

5. "How hard can getting $C^2$ smoothness be"

    Constraints:
    $$
    f'(0) = 1 \\
    f(L) = 1 \\
    f'(L) = 0 \\
    f''(L) = 0
    $$
    Let
    $$
    f(x) = a_0 + a_1x^1 + a_2x^2 + a_3x^3 + a_4x^4 + a_5x^5 + a_6x^6 + a_7x^7
    $$
    But we want $f$ odd, so $a_0 = a_2 = a_4 = a_6 = 0$. $f'(0) = 1$ immediately yields $a_1 = 1$. Computing derivatives of $f$ and applying the remaining constraints we get:
    $$
    L+a_3L^3+a_5L^5+a_7L^7=1 \\
    1+3a_3L^2+5a_5L^4+7a_7L^6=0 \\
    6a_3L+20a_5L^3+42a_7L^5=0
    $$ \
    which is a system of three linear equations in three unknowns ($a_3, a_5, a_7$). Solve, for example with $L=2$, to obtain
    $$
    f(x) = x - \frac{13}{64}x^3 + \frac{3}{128}x^5 - \frac{1}{1024}x^7
    $$
    Since $f'(L) = f''(L) = 0$, and after clamping $f(x)$ is constant for $x>L$, $f$ is $C^2$ at the saturation points. Elsewhere, $f$ is trivially $C^{\infty}$. Verification of the constraints by computing the derivatives of $f$ is left as an exercise for the reader.

    Maybe I'm getting a little sentimental because I spent so much time putting pencil to paper for this one, but I feel like it would be a nice flagship soft clipping curve for the plugin.