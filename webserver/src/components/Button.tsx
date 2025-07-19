import { ComponentProps, forwardRef } from 'react';
import { cva, type VariantProps } from 'class-variance-authority';
import { FiLoader } from 'react-icons/fi';
const buttonStyles = cva(
    'inline-flex items-center justify-center rounded-lg font-semibold transition-all focus:outline-none focus:ring-2 focus:ring-primary/50 disabled:opacity-50 disabled:pointer-events-none shadow-sm cursor-pointer',
    {
        variants: {
            variant: {
                primary: 'bg-button-primary text-white hover:bg-button-primary-hover shadow-lg',
                outline: 'border-2 border-secondary text-secondary hover:bg-secondary/10',
                ghost: 'hover:bg-gray-100 dark:hover:bg-gray-800',
                link: 'underline-offset-4 hover:underline text-primary',
            },
            size: {
                sm: 'px-4 py-2 text-sm',
                md: 'px-8 py-3 text-base',
                lg: 'px-10 py-4 text-lg',
            },
            fullWidth: {
                true: 'w-full',
                false: 'w-auto',
            },
        },
        defaultVariants: {
            variant: 'primary',
            size: 'md',
            fullWidth: true,
        },
    }
);

type ButtonProps = VariantProps<typeof buttonStyles> &
    ComponentProps<'button'> & {
        loading?: boolean;
        icon?: React.ReactNode;
    };

export const Button = forwardRef<HTMLButtonElement, ButtonProps>(
    (
        {
            variant,
            size,
            fullWidth,
            className,
            type = 'button',
            loading = false,
            icon,
            children,
            ...props
        },
        ref
    ) => {
        return (
            <button
                ref={ref}
                type={type}
                className={buttonStyles({ variant, size, fullWidth, className })}
                disabled={loading || props.disabled}
                {...props}
            >
                {icon && <span className={children ? 'mr-2' : ''}>{icon}</span>}
                {children}
                {loading && (
                    <span className="ml-2 animate-spin">
                        <FiLoader />
                    </span>
                )}
            </button>
        );
    }
);

Button.displayName = 'Button';
