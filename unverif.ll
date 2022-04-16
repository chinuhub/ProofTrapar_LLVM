; ModuleID = 'unverif.c'
source_filename = "unverif.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@lock = dso_local global i32 0, align 4
@r = dso_local global i32 0, align 4
@s = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  br label %2

2:                                                ; preds = %6, %0
  %3 = cmpxchg i32* @lock, i32 0, i32 1 seq_cst seq_cst
  %4 = extractvalue { i32, i1 } %3, 1
  %5 = xor i1 %4, true
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %2

7:                                                ; preds = %2
  %8 = load i32, i32* @r, align 4
  %9 = add nsw i32 %8, 1
  store i32 %9, i32* @r, align 4
  store i32 0, i32* @lock, align 4
  %10 = load i32, i32* @r, align 4
  %11 = icmp eq i32 %10, 1
  br i1 %11, label %12, label %23

12:                                               ; preds = %7
  br label %13

13:                                               ; preds = %12, %22
  %14 = load i32, i32* @s, align 4
  %15 = add nsw i32 %14, 1
  store i32 %15, i32* @s, align 4
  %16 = load i32, i32* %1, align 4
  %17 = add nsw i32 %16, 1
  store i32 %17, i32* %1, align 4
  %18 = load i32, i32* @s, align 4
  %19 = load i32, i32* %1, align 4
  %20 = icmp eq i32 %18, %19
  br i1 %20, label %22, label %21

21:                                               ; preds = %13
  call void @__ASSERT()
  br label %22

22:                                               ; preds = %21, %13
  br label %13

23:                                               ; preds = %7
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  br label %2

2:                                                ; preds = %6, %0
  %3 = cmpxchg i32* @lock, i32 0, i32 1 seq_cst seq_cst
  %4 = extractvalue { i32, i1 } %3, 1
  %5 = xor i1 %4, true
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %2

7:                                                ; preds = %2
  %8 = load i32, i32* @r, align 4
  %9 = add nsw i32 %8, 1
  store i32 %9, i32* @r, align 4
  store i32 0, i32* @lock, align 4
  %10 = load i32, i32* @r, align 4
  %11 = icmp eq i32 %10, 1
  br i1 %11, label %12, label %23

12:                                               ; preds = %7
  br label %13

13:                                               ; preds = %12, %22
  %14 = load i32, i32* @s, align 4
  %15 = add nsw i32 %14, 1
  store i32 %15, i32* @s, align 4
  %16 = load i32, i32* %1, align 4
  %17 = add nsw i32 %16, 1
  store i32 %17, i32* %1, align 4
  %18 = load i32, i32* @s, align 4
  %19 = load i32, i32* %1, align 4
  %20 = icmp eq i32 %18, %19
  br i1 %20, label %22, label %21

21:                                               ; preds = %13
  call void @__ASSERT()
  br label %22

22:                                               ; preds = %21, %13
  br label %13

23:                                               ; preds = %7
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
